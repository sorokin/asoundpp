cmake_minimum_required (VERSION 2.6)

include_directories("${SOUNDIO_SOURCE_DIR}")

add_executable(speex_echo
   speex_echo.cpp
)

target_link_libraries(speex_echo soundio asound speex)
