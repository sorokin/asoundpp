cmake_minimum_required (VERSION 2.6)

include_directories("${SOUNDIO_SOURCE_DIR}")

add_executable(info
   info.cpp
)

target_link_libraries(info soundio asound)
