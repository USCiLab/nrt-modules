cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH /usr/share/nrt/CMakeModules/)
include(NRTHelpers)

nrt_build_module(StereoDepthModule
  SOURCES StereoDepthModule.C
  LINK_LIBS opencv_core opencv_calib3d opencv_imgproc)
