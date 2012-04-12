cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH /usr/share/nrt/CMakeModules/)
include(NRTHelpers)

nrt_build_module(StereoRectifierModule
  SOURCES StereoRectifierModule.C
  LINK_LIBS opencv_core opencv_calib3d opencv_imgproc)

add_executable(calibrateCameras calibrateCameras.cpp)
target_link_libraries(calibrateCameras nrtCore nrtRobotics nrtImageProc 
                      nrtPointCloud boost_thread boost_serialization 
                      opencv_core opencv_calib3d opencv_imgproc)
