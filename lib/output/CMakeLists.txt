cmake_minimum_required (VERSION 2.6)

include_directories("${ALSA_SOURCE_DIR}")
include_directories("${INPUT_SOURCE_DIR}")

project(OUTPUT)

add_library(output
   wave_writer.cpp
   wave_writer.hpp
   output_device.cpp
   output_device.hpp
)
