cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH /usr/share/nrt/CMakeModules/)
include(NRTHelpers)

nrt_build_module(TransformVisualizerModule
  SOURCES TransformVisualizerModule.C)

FIND_PACKAGE(GTK2 COMPONENTS gtk REQUIRED)
include_directories(${GTK2_INCLUDE_DIRS})
target_link_libraries(TransformVisualizerModule ${GTK2_LIBRARIES})
target_link_libraries(TransformVisualizerModule -lgthread-2.0)
target_link_libraries(TransformVisualizerModule -lgdk_pixbuf-2.0)


find_package(PkgConfig)
pkg_check_modules(CAIRO QUIET cairo REQUIRED)
include_directories(${CAIRO_INCLUDEDIR})
target_link_libraries(TransformVisualizerModule -lcairo)
message(STATUS "LIBS: ${CAIRO_LIBRARIES}")

