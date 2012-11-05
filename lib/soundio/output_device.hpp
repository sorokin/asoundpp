#ifndef SOUNDIO_OUTPUT_DEVICE_H
#define SOUNDIO_OUTPUT_DEVICE_H

#include "format.hpp"
#include <boost/noncopyable.hpp>
#include "asoundpp.hpp"

struct output_device : boost::noncopyable
{
    output_device(frame_format fmt);


    // blocks execution if buffer of operating system is full
    // buf must contain at least fmt.frame_size() * number_of_frames bytes
    void write(void const* buf,
               size_t number_of_frames);

private:
    asound::pcm::device d;
};

#endif
