cmake_minimum_required (VERSION 2.6)

include_directories("${ALSA_SOURCE_DIR}")
include_directories("${INPUT_SOURCE_DIR}")

add_executable(waveplay
   waveplay.cpp
)

target_link_libraries(waveplay asound input alsa)
