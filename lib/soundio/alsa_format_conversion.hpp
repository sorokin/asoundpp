#ifndef SOUNDIO_ALSA_FORMAT_CONVERSION_HPP
#define SOUNDIO_ALSA_FORMAT_CONVERSION_HPP

#include <alsa/asoundlib.h>
#include "format.hpp"

snd_pcm_format_t sample_format_to_alsa_format(sample_format sample_fmt);

#endif
