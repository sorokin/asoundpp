cmake_minimum_required (VERSION 2.6)

include_directories("${INPUT_SOURCE_DIR}" "${OUTPUT_SOURCE_DIR}")

add_executable(flac2wave
   flac2wave.cpp
)

target_link_libraries(flac2wave input output)
