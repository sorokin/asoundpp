cmake_minimum_required (VERSION 2.6)

project(SOUNDIO)

add_library(soundio
   asoundpp.cpp
   asoundpp.hpp
   alsa_format_conversion.cpp
   alsa_format_conversion.hpp
   asoundpp.hpp
   format.hpp
   format.cpp
   input_device.cpp
   input_device.hpp
   output_device.hpp
   output_device.cpp
   speex_decoder.hpp
   speex_decoder.cpp
   speex_encoder.hpp
   speex_encoder.cpp
   speex_profile.hpp
)
