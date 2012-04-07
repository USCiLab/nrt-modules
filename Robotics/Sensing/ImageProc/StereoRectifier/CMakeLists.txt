cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH /usr/share/nrt/CMakeModules/)
include(NRTHelpers)

find_package(Eigen3 QUIET)
if(EIGEN3_FOUND)
  include_directories(${EIGEN3_INCLUDE_DIR})
endif()
add_definitions(-std=c++0x -g)
add_executable(calibrateCameras calibrateCameras.cpp)
target_link_libraries(calibrateCameras nrtCore nrtRobotics nrtImageProc nrtPointCloud boost_thread boost_serialization opencv_core opencv_calib3d opencv_imgproc)
