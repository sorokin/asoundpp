#include "input_device.hpp"

input_device::input_device(std::string const& device_name, format const& fmt)
    : fmt_(fmt)
    , d_(device_name.c_str(), SND_PCM_STREAM_CAPTURE, 0)
{
    d_.set_params(fmt.fmt, SND_PCM_ACCESS_RW_INTERLEAVED, fmt.channels, fmt.sample_rate, true, 500000);
}

format input_device::get_format()
{
    return fmt_;
}

size_t input_device::get_available()
{
    return d_.avail_update();
}

void input_device::read(void* buf, size_t size)
{
    try
    {
        d_.readi(buf, size);
    }
    catch (asound::pcm::underrun const&)
    {
        d_.prepare();
    }
}
