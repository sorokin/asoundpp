cmake_minimum_required (VERSION 2.6)

project(INPUT)

find_package(Boost COMPONENTS iostreams REQUIRED)

include_directories("${ALSA_SOURCE_DIR}")

add_library(input
   flac.cpp
   input_device.cpp
   input_stream.cpp
   input_stream.h
   sine_wave.cpp
   wave.cpp
)

target_link_libraries(input FLAC ${Boost_LIBRARIES})
