cmake_minimum_required (VERSION 2.6)

project(ALSA)

add_library(alsa
   asound_async.hpp
   asoundpp.cpp
   asoundpp.hpp
   operation_cancelation.h
)
