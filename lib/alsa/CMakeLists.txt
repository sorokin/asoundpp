cmake_minimum_required (VERSION 2.6)

project(ALSA)

add_custom_target(alsa
   asound_async.hpp
   asoundpp.hpp
   frame.hpp
   operation_cancelation.h
)
