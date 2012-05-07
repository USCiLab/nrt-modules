cmake_minimum_required(VERSION 2.8)

file(GLOB LWSSources libs/libwebsockets/*.c)
add_executable(test-server test-server.c ${LWSSources})

find_package(ZLIB QUIET)
if(ZLIB_FOUND)
  target_link_libraries(test-server ${ZLIB_LIBRARIES})
  include_directories(${ZLIB_INCLUDE_DIRS})
endif(ZLIB_FOUND)

