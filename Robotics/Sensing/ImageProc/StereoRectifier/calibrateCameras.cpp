#include <nrt/Core/Model/Manager.H>
#include <nrt/ImageProc/IO/ImageSource/V4L2ImageSource.H>
#include <nrt/ImageProc/IO/ImageSink/DisplayImageSink.H>
#include <nrt/ImageProc/Drawing/CutPaste.H>
#include <nrt/ImageProc/Drawing/Text.H>
#include <nrt/ImageProc/IO/ImageSink/ImageWriters/ImageWriters.H>
#include <nrt/ImageProc/LibraryConversions/OpenCV.H>

using namespace nrt;

void keyCallback(int key)
{
  NRT_INFO("Key Pressed: " << char(key));
  switch(key)
  {
    case ' ':
      break;
    case 'p':
      break;
    case 'q':
      break;
  }
}

int main(int const argc, char const ** argv)
{
  Manager mgr(argc, argv);

  nrt::Parameter<float> checkSizeParam(
      nrt::ParameterDef<float>("checksize", "The length of a checkerboard square in mm", 27.0), &mgr);

  nrt::Parameter<Dims<int>> checkDimsParam(
      nrt::ParameterDef<Dims<int>>("checkdims", "The number of inner corners in the checkerboard", Dims<int>(8, 6)), &mgr);

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

  auto sink = std::make_shared<DisplayImageSink>("display");
  mgr.addSubComponent(sink);
  sink->setKeyCallback(keyCallback);

  mgr.launch();

  cv::Size const checkDims(checkDimsParam.getVal().width(), checkDimsParam.getVal().height());

  // Create the "object points" vector
  float const checkSize = checkSizeParam.getVal();
  std::vector<cv::Point3f> objectPoints;
  for(int x = 0; x<checkDimsParam.getVal().width(); ++x)
    for(int y = 0; y<checkDimsParam.getVal().height(); ++y)
      objectPoints.push_back(cv::Point3f(x*checkSize, y*checkSize, 0));

  int num = 0;
  while(leftCam->ok())
  {
    // Grab the newest camera images
    auto leftimg  = leftCam->in().convertTo<PixRGB<byte>>();
    auto rightimg = rightCam->in().convertTo<PixRGB<byte>>();

    // Convert the images to grayscale cv::Mat
    cv::Mat leftmat  = copyImage2CvMat(Image<PixGray<byte>>(leftimg));
    cv::Mat rightmat = copyImage2CvMat(Image<PixGray<byte>>(rightimg));

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

    // If we have found all of the corners
    if(leftCorners.size() == 48 && rightCorners.size() == 48)
    {
    }


    // Display the two images side-by-side
    auto combinedimg = concatX(leftimg, rightimg);
    Image<PixRGB<byte>> displayImage(combinedimg.width(), combinedimg.height()+100, nrt::ImageInitPolicy::Zeros);
    paste(displayImage, combinedimg, Point2D<int>(0,0));
    drawText(displayImage, Point2D<int>(5, combinedimg.height()+20), "<space> : Take a picture");
    drawText(displayImage, Point2D<int>(5, combinedimg.height()+60), "q       : Quit (and process all pictures)");
    sink->out(GenericImage(displayImage), "Left/Right Images");
  }
}


