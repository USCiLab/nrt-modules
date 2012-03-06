cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH /usr/share/nrt/CMakeModules/)
include(NRTHelpers)

nrt_build_module(MotionPlanner2dModule
  SOURCES MotionPlanner2dModule.C)
