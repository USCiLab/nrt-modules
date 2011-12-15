cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH /usr/share/nrt/CMakeModules/)
include(NRTHelpers)

nrt_build_module(iRobotCreate 
	LINK_LIBS serial 
	SOURCES iRobotCreate.C)
