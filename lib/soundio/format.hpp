#ifndef INPUT_STREAM_H
#define INPUT_STREAM_H

#include <cstdlib>
#include <alsa/asoundlib.h>

struct format
{
  format();
  format(unsigned sample_rate, unsigned channels, snd_pcm_format_t fmt);
  size_t frame_size() const;

  unsigned sample_rate;
  unsigned channels;
  snd_pcm_format_t fmt;
};

#endif // INPUT_STREAM_H
