cmake_minimum_required (VERSION 2.6)

include_directories("${ALSA_SOURCE_DIR}")
include_directories("${INPUT_SOURCE_DIR}")

find_package(Boost COMPONENTS system REQUIRED)

add_executable(waveplay_async
   waveplay_async.cpp
)

target_link_libraries(waveplay_async asound input ${Boost_LIBRARIES})
