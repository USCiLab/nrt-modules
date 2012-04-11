cmake_minimum_required(VERSION 2.8) 

set(CMAKE_MODULE_PATH /usr/share/nrt/CMakeModules/)
include(NRTHelpers)

nrt_build_module(OpenNISourceModule
  SOURCES OpenNISourceModule.C)

FIND_PACKAGE(OpenNI QUIET REQUIRED)
include_directories(${OPENNI_INCLUDE_DIR})
target_link_libraries(OpenNISourceModule ${OPENNI_LIBRARIES})
target_link_libraries(OpenNISourceModule -lOpenNI)
add_definitions(-Wno-unknown-pragmas)
