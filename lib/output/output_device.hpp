#ifndef OUTPUT_DEVICE_H
#define OUTPUT_DEVICE_H

#include "input_stream.h"
#include <boost/noncopyable.hpp>
#include "asoundpp.hpp"

struct output_device : boost::noncopyable
{
    output_device(std::string const& device_name, input_stream::format fmt);

    void write(void const* buf, size_t number_of_frames);

private:
    asound::pcm::device d;
};

#endif // OUTPUT_DEVICE_H
