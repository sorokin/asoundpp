cmake_minimum_required (VERSION 2.6)

project(INPUT)

add_library(input
   flac.cpp
   input_stream.cpp
   input_stream.h
   sine_wave.cpp
   wave.cpp
)

target_link_libraries(input FLAC)
