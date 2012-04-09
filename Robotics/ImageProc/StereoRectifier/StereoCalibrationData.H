#ifndef NRT_MODULES_ROBOTICS_SENSING_IMAGEPROC_STEREORECTIFIER_STEREOCALIBRATIONDATA_H
#define NRT_MODULES_ROBOTICS_SENSING_IMAGEPROC_STEREORECTIFIER_STEREOCALIBRATIONDATA_H

#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class StereoCalibrationData
{
  public:
    StereoCalibrationData()
    {
      leftIntrinsics  = cv::Mat::eye(3, 3, CV_64F);
      rightIntrinsics = cv::Mat::eye(3, 3, CV_64F);
    }

    StereoCalibrationData(std::string const & filename)
    {
      cv::FileStorage fs(filename, CV_STORAGE_READ);
      if(!fs.isOpened()) throw std::runtime_error("Can not open file");

      fs["LeftIntrinsics"]  >> leftIntrinsics;
      fs["LeftDistortion"]  >> leftDistortion;
      fs["RightIntrinsics"] >> rightIntrinsics;
      fs["RightDistortion"] >> rightDistortion;
      fs["R"]               >> R;
      fs["T"]               >> T;
      fs["R1"]              >> R1;
      fs["R2"]              >> R2;
      fs["P1"]              >> P1;
      fs["P2"]              >> P2;
      fs["Q"]               >> Q;
      fs["E"]               >> E;
      fs["F"]               >> F;
      fs.release();
    }

    void save(std::string const & filename)
    {
      cv::FileStorage fs(filename, CV_STORAGE_WRITE);
      if(!fs.isOpened()) throw std::runtime_error("Can not open file");

      fs << "LeftIntrinsics"  << leftIntrinsics;
      fs << "LeftDistortion"  << leftDistortion;
      fs << "RightIntrinsics" << rightIntrinsics;
      fs << "RightDistortion" << rightDistortion;
      fs << "R"               << R;
      fs << "T"               << T;
      fs << "R1"              << R1;
      fs << "R2"              << R2;
      fs << "P1"              << P1;
      fs << "P2"              << P2;
      fs << "Q"               << Q;
      fs << "E"               << E;
      fs << "F"               << F;
      fs.release();
    }

    cv::Mat leftIntrinsics;
    cv::Mat leftDistortion;
    cv::Mat rightIntrinsics;
    cv::Mat rightDistortion;
    cv::Mat R;
    cv::Mat T;
    cv::Mat R1;
    cv::Mat R2;
    cv::Mat P1;
    cv::Mat P2;
    cv::Mat Q;
    cv::Mat E;
    cv::Mat F;
};

#endif // NRT_MODULES_ROBOTICS_SENSING_IMAGEPROC_STEREORECTIFIER_STEREOCALIBRATIONDATA_H

