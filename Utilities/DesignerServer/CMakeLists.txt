cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH /usr/share/nrt/CMakeModules/)
include(NRTHelpers)

file(GLOB LWSSources libs/libwebsockets/*.c)
nrt_build_module(DesignerServerModule
  SOURCES DesignerServerModule.C ${LWSSources} Server.C
  LINK_LIBS json)

find_package(ZLIB QUIET)
if(ZLIB_FOUND)
  target_link_libraries(DesignerServerModule ${ZLIB_LIBRARIES})
  include_directories(${ZLIB_INCLUDE_DIRS})
endif(ZLIB_FOUND)

