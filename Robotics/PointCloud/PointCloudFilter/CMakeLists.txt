cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH /usr/share/nrt/CMakeModules/)
include(NRTHelpers)

FILE(GLOB Filters "Filters/*.C")
nrt_build_module(PointCloudFilterModule
  SOURCES PointCloudFilterModule.C ${Filters})
