#ifndef SOUNDIO_INPUT_DEVICE_H
#define SOUNDIO_INPUT_DEVICE_H

#include "asoundpp.hpp"
#include "format.hpp"

struct input_device
{
    input_device(frame_format const& fmt);
    input_device(input_device const&) = delete;
    input_device& operator=(input_device const&) = delete;

    frame_format get_format();
    size_t get_available();

    // blocks execution until buffer are filled completely
    // use get_available() to get the number of frames that can be read without blocking
    // buf must contain at least get_format().frame_size() * number_of_frames bytes
    void read(void* buf,
              size_t number_of_frames);


private:
    frame_format fmt_;
    asound::pcm::device d_;
};

#endif
