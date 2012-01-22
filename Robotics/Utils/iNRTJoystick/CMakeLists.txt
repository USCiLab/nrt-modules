cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH /home/sagar/workspace/nrt/CMakeModules)
include(NRTHelpers)

nrt_build_module(iNRTJoystick
  SOURCES iNRTJoystick.C)
