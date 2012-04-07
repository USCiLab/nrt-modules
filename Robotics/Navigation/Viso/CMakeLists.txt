cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH /usr/share/nrt/CMakeModules/)
include(NRTHelpers)

include_directories("libviso2")

# use sse3 instruction set
SET(CMAKE_CXX_FLAGS "-msse3")

FILE(GLOB LIBVISO2_SRC_FILES "libviso2/*.cpp")

nrt_build_module(Viso
  SOURCES Viso.C ${LIBVISO2_SRC_FILES})
