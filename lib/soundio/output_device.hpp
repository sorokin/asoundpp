#ifndef SOUNDIO_OUTPUT_DEVICE_H
#define SOUNDIO_OUTPUT_DEVICE_H

#include "format.hpp"
#include "asoundpp.hpp"

struct output_device
{
    output_device(frame_format fmt);
    output_device(output_device const&) = delete;
    output_device& operator=(output_device const&) = delete;


    // blocks execution if buffer of operating system is full
    // buf must contain at least fmt.frame_size() * number_of_frames bytes
    void write(void const* buf,
               size_t number_of_frames);

private:
    asound::pcm::device d;
};

#endif
