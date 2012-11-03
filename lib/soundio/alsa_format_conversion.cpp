#include "alsa_format_conversion.hpp"

#include <stdexcept>

snd_pcm_format_t sample_format_to_alsa_format(sample_format sample_fmt)
{
    switch (sample_fmt)
    {
    case SOUNDIO_SAMPLE_FORMAT_UNKNOWN:
        return SND_PCM_FORMAT_UNKNOWN;

    case SOUNDIO_SAMPLE_FORMAT_U8:
        return SND_PCM_FORMAT_U8;
    case SOUNDIO_SAMPLE_FORMAT_S8:
        return SND_PCM_FORMAT_S8;

    case SOUNDIO_SAMPLE_FORMAT_U16:
        return SND_PCM_FORMAT_U16;
    case SOUNDIO_SAMPLE_FORMAT_S16:
        return SND_PCM_FORMAT_S16;

    case SOUNDIO_SAMPLE_FORMAT_U24:
        return SND_PCM_FORMAT_U24;
    case SOUNDIO_SAMPLE_FORMAT_S24:
        return SND_PCM_FORMAT_S24;

    case SOUNDIO_SAMPLE_FORMAT_U32:
        return SND_PCM_FORMAT_U32;
    case SOUNDIO_SAMPLE_FORMAT_S32:
        return SND_PCM_FORMAT_S32;

    default:
        throw std::runtime_error("invalid sample_format value");
    }
}
