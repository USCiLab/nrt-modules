# CMakeLists.txt for NRT module FreenectSourceModule
# To compile this module, simply type:
#   cmake .
#   make
#   make manifest
#   make nrt

cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH /usr/share/nrt/CMakeModules/ ${CMAKE_ROOT}/Modules /usr/CMakeModules)

include(NRTHelpers)
include_directories(/usr/local/include/libfreenect)

nrt_build_module(FreenectSourceModule
  SOURCES FreenectSourceModule.C
  LINK_LIBS freenect freenect_sync
)
