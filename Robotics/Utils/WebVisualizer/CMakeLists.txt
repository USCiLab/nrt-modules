cmake_minimum_required(VERSION 2.8)

set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} mongoose/)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} mongoose/)

set(CMAKE_MODULE_PATH /home/sagar/workspace/nrt/CMakeModules)
include(NRTHelpers)


nrt_build_module(WebVisualizer
  SOURCES WebVisualizer.C mongoose/mongoose.c
  )
