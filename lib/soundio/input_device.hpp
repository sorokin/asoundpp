#ifndef INPUT_DEVICE_H
#define INPUT_DEVICE_H

#include <boost/noncopyable.hpp>
#include "asoundpp.hpp"
#include "format.hpp"

struct input_device : boost::noncopyable
{
    input_device(std::string const& device_name, format const& fmt);

    format get_format();
    size_t get_available();

    void read(void* buf, size_t size);

private:
    format fmt_;
    asound::pcm::device d_;
};

#endif // INPUT_DEVICE_H
