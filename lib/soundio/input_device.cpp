#include "input_device.hpp"
#include "alsa_format_conversion.hpp"

input_device::input_device(frame_format const& fmt)
    : fmt_(fmt)
    , d_("default", SND_PCM_STREAM_CAPTURE, 0)
{
    d_.set_params(sample_format_to_alsa_format(fmt.sample_fmt), SND_PCM_ACCESS_RW_INTERLEAVED, fmt.channels, fmt.rate, true, 500000);
}

frame_format input_device::get_format()
{
    return fmt_;
}

size_t input_device::get_available()
{
    return d_.avail_update();
}

void input_device::read(void* buf, size_t number_of_frames)
{
    try
    {
        d_.readi(buf, number_of_frames);
    }
    catch (asound::pcm::underrun const&)
    {
        d_.prepare();
    }
}
