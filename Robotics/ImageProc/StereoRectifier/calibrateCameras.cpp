#include <nrt/Core/Model/Manager.H>
#include <nrt/ImageProc/IO/ImageSource/V4L2ImageSource.H>
#include <nrt/ImageProc/IO/ImageSink/DisplayImageSink.H>
#include <nrt/ImageProc/Drawing/CutPaste.H>
#include <nrt/ImageProc/Drawing/Text.H>
#include <nrt/ImageProc/IO/ImageSink/ImageWriters/ImageWriters.H>
#include <nrt/ImageProc/LibraryConversions/OpenCV.H>
#include "StereoCalibrationData.H"

using namespace nrt;

int main(int const argc, char const ** argv)
{
  Manager mgr(argc, argv);

  nrt::Parameter<float> checkSizeParam(
      nrt::ParameterDef<float>("checksize", "The length of a checkerboard square in mm", 27.0), &mgr);

  nrt::Parameter<Dims<int>> checkDimsParam(
      nrt::ParameterDef<Dims<int>>("checkdims", "The number of inner corners in the checkerboard", Dims<int>(8, 6)), &mgr);

  nrt::Parameter<std::string> calibrationFileParam(
      nrt::ParameterDef<std::string>("calibrationfile", "The calibration file filename", "calibration.yml"), &mgr);

  auto leftCam = std::make_shared<V4L2ImageSource>("leftcamera");
  mgr.addSubComponent(leftCam);
  leftCam->setParamVal("device", std::string("/dev/video1"));
  leftCam->setParamVal("autofocus", false);
  leftCam->setParamVal("focus", 0);

  auto rightCam = std::make_shared<V4L2ImageSource>("rightcamera");
  mgr.addSubComponent(rightCam);
  rightCam->setParamVal("device", std::string("/dev/video2"));
  rightCam->setParamVal("autofocus", false);
  rightCam->setParamVal("focus", 0);

  bool timeToQuit   = false;
  bool takeAPicture = false;
  auto sink = std::make_shared<DisplayImageSink>("display");
  mgr.addSubComponent(sink);
  sink->setKeyCallback([&timeToQuit, &takeAPicture](int key)
      {
        switch(key)
        {
          case ' ': // Take a picture
            takeAPicture = true;
            break;
          case 'q': // Quit
            timeToQuit = true;
            break;
        }
      });


  mgr.launch();

  cv::Size const checkDims(checkDimsParam.getVal().width(), checkDimsParam.getVal().height());

  /////////////////////////////////////////////////////////////////////////
  // GRAB IMAGES
  /////////////////////////////////////////////////////////////////////////

  std::vector<std::vector<cv::Point2f>> leftDetectedPoints;
  std::vector<std::vector<cv::Point2f>> rightDetectedPoints;
  cv::Size imageSize;
  while(leftCam->ok() && rightCam->ok() && !timeToQuit)
  {
    // Grab the newest camera images
    auto leftimg  = leftCam->in().convertTo<PixRGB<byte>>();
    auto rightimg = rightCam->in().convertTo<PixRGB<byte>>();

    // Convert the images to grayscale cv::Mat
    cv::Mat leftmat  = copyImage2CvMat(Image<PixGray<byte>>(leftimg));
    cv::Mat rightmat = copyImage2CvMat(Image<PixGray<byte>>(rightimg));
    imageSize = leftmat.size();

    // Find the corners in the left image
    std::vector<cv::Point2f> leftCorners;
    if(cv::findChessboardCorners(leftmat, checkDims, leftCorners,
          cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK))
      cv::cornerSubPix(leftmat, leftCorners, cv::Size(11, 11), cv::Size(-1, -1),
          cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

    // Find the corners in the right image
    std::vector<cv::Point2f> rightCorners;
    if(cv::findChessboardCorners(rightmat, checkDims, rightCorners,
          cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK))
      cv::cornerSubPix(rightmat, rightCorners, cv::Size(11, 11), cv::Size(-1, -1),
          cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

    // Draw the left corners
    cv::Mat leftcolor = copyImage2CvMat(leftimg);
    cv::drawChessboardCorners(leftcolor,  checkDims, leftCorners,  leftCorners.size()  == 48);
    leftimg = copyCvMat2Image<PixRGB<byte>>(leftcolor);

    // Draw the right corners
    cv::Mat rightcolor = copyImage2CvMat(rightimg);
    cv::drawChessboardCorners(rightcolor,  checkDims, rightCorners,  rightCorners.size()  == 48);
    rightimg = copyCvMat2Image<PixRGB<byte>>(rightcolor);

    if(takeAPicture)
    {
      // If we have found all of the corners
      if(leftCorners.size() == 48 && rightCorners.size() == 48)
      {
        leftDetectedPoints.push_back(leftCorners);
        rightDetectedPoints.push_back(rightCorners);
        NRT_INFO("Click");
      }
      else NRT_WARNING("The checkerboard is not fully visible in both frames!");

      takeAPicture = false;
    }

    // Display the two images side-by-side
    auto combinedimg = concatX(leftimg, rightimg);
    Image<PixRGB<byte>> displayImage(combinedimg.width(), combinedimg.height()+100, nrt::ImageInitPolicy::Zeros);
    paste(displayImage, combinedimg, Point2D<int>(0,0));
    drawText(displayImage, Point2D<int>(5, combinedimg.height()+00), "<space> : Take a picture");
    drawText(displayImage, Point2D<int>(5, combinedimg.height()+20), "   q    : Quit (and process all pictures)");
    drawText(displayImage, Point2D<int>(5, combinedimg.height()+60), sformat("%d Pictures Taken", leftDetectedPoints.size()));
    sink->out(GenericImage(displayImage), "Left/Right Images");
  }

  /////////////////////////////////////////////////////////////////////////
  // PERFORM CALIBRATION
  /////////////////////////////////////////////////////////////////////////

  NRT_INFO("Calibrating, Please Wait...");

  // Create the "object points" vector
  float const checkSize = checkSizeParam.getVal();
  std::vector<cv::Point3f> objectPoints;
  for(int y = 0; y<checkDimsParam.getVal().height(); ++y)
    for(int x = 0; x<checkDimsParam.getVal().width(); ++x)
      objectPoints.push_back(cv::Point3f(y*checkSize, x*checkSize, 0));

  StereoCalibrationData calibdata;

  // Compute the intrinsic parameters
  double error = cv::stereoCalibrate(std::vector<std::vector<cv::Point3f>>(leftDetectedPoints.size(), objectPoints),
      leftDetectedPoints, rightDetectedPoints, 
      calibdata.leftIntrinsics, calibdata.leftDistortion,
      calibdata.rightIntrinsics, calibdata.rightDistortion,
      imageSize, calibdata.R, calibdata.T, calibdata.E, calibdata.F,
      cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, 1e-6), 0);

  NRT_INFO("Calibrated with an error of " << error);

  // Compute the extrinsic parameters
  cv::stereoRectify(calibdata.leftIntrinsics, calibdata.leftDistortion, calibdata.rightIntrinsics, calibdata.rightDistortion,
      imageSize, calibdata.R, calibdata.T, calibdata.R1, calibdata.R2, calibdata.P1, calibdata.P2, calibdata.Q);

  // Save the calibration file
  calibdata.save(calibrationFileParam.getVal());


  /////////////////////////////////////////////////////////////////////////
  // DISPLAY CALIBRATED IMAGES AND STEREO
  /////////////////////////////////////////////////////////////////////////
  cv::Mat rmap[2][2];
  initUndistortRectifyMap(calibdata.leftIntrinsics, calibdata.leftDistortion,
      calibdata.R, calibdata.P1, imageSize, CV_16SC2, rmap[0][0], rmap[0][1]);
  initUndistortRectifyMap(calibdata.rightIntrinsics, calibdata.rightDistortion,
      calibdata.R, calibdata.P1, imageSize, CV_16SC2, rmap[1][0], rmap[1][1]);

  int cn = 3;
  cv::StereoSGBM sgbm;
  sgbm.P1 = 8*cn*sgbm.SADWindowSize*sgbm.SADWindowSize;
  sgbm.P2 = 32*cn*sgbm.SADWindowSize*sgbm.SADWindowSize;
  sgbm.disp12MaxDiff = 1;
  sgbm.minDisparity = 0;
  sgbm.numberOfDisparities = ((imageSize.width/8) + 15) & -16;
  sgbm.uniquenessRatio = 15;
  sgbm.speckleWindowSize = 200;
  sgbm.speckleRange = 32;
  sgbm.fullDP = true;
  sgbm.SADWindowSize = 11;

  timeToQuit = false;
  while(leftCam->ok() && rightCam->ok() && !timeToQuit)
  {
    // Grab the newest camera images
    auto leftimg  = leftCam->in().convertTo<PixRGB<byte>>();
    auto rightimg = rightCam->in().convertTo<PixRGB<byte>>();

    // Convert the images to cv::Mat
    cv::Mat leftmat  = copyImage2CvMat(Image<PixRGB<byte>>(leftimg));
    cv::Mat rightmat = copyImage2CvMat(Image<PixRGB<byte>>(rightimg));

    // Remap the images
    cv::Mat leftRemapped, rightRemapped;
    cv::remap(leftmat, leftRemapped, rmap[0][0], rmap[0][1], CV_INTER_LINEAR);
    cv::remap(rightmat, rightRemapped, rmap[1][0], rmap[1][1], CV_INTER_LINEAR);

    cv::Mat disparity;
    sgbm(leftRemapped, rightRemapped, disparity); 

    cv::Mat floatDisparity;
    disparity.convertTo(floatDisparity, CV_32F, 255.0 / (sgbm.numberOfDisparities*16.0));
    Image<PixGray<float>> disparityImg = copyCvMat2Image<PixGray<float>>(floatDisparity);
    sink->out(GenericImage(disparityImg), "Disparity");

    auto combinedimg = concatX(copyCvMat2Image<PixRGB<byte>>(leftRemapped), copyCvMat2Image<PixRGB<byte>>(rightRemapped));
    Image<PixRGB<byte>> displayImage(combinedimg.width(), combinedimg.height()+50, nrt::ImageInitPolicy::Zeros);
    paste(displayImage, combinedimg, Point2D<int>(0,0));
    drawText(displayImage, Point2D<int>(5, combinedimg.height()+0), "q    : Quit (and process all pictures)");
    sink->out(GenericImage(displayImage), "Left/Right Images");
  }

}


