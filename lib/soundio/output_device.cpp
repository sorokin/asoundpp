#include "output_device.hpp"
#include "alsa_format_conversion.hpp"

output_device::output_device(std::string const& device_name, frame_format fmt)
    : d(device_name.c_str(), SND_PCM_STREAM_PLAYBACK, 0)
{
    d.set_params(sample_format_to_alsa_format(fmt.sample_fmt),
                 SND_PCM_ACCESS_RW_INTERLEAVED,
                 fmt.channels,
                 fmt.rate,
                 true,
                 500000);
}

void output_device::write(void const* buf, size_t number_of_frames)
{
    try
    {
        d.writei(buf, number_of_frames);
    }
    catch (asound::pcm::underrun const& e)
    {
        d.prepare();
    }
}
