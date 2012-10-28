cmake_minimum_required (VERSION 2.6)

include_directories("${ALSA_SOURCE_DIR}")
include_directories("${INPUT_SOURCE_DIR}")
include_directories("${OUTPUT_SOURCE_DIR}")

add_executable(speex_echo
   speex_echo.cpp
)

target_link_libraries(speex_echo alsa input output asound speex)
