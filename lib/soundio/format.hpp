#ifndef INPUT_STREAM_H
#define INPUT_STREAM_H

#include <cstdlib>
#include <alsa/asoundlib.h>

struct format
{
  format();
  format(unsigned sample_rate,  // (samples per second) e.g. 44100
         unsigned channels,     // e.g. 2
         snd_pcm_format_t fmt); // e.g. SND_PCM_FORMAT_S16

  // sample_size * channels
  size_t frame_size() const;

  unsigned sample_rate;
  unsigned channels;
  snd_pcm_format_t fmt;
};

#endif // INPUT_STREAM_H
