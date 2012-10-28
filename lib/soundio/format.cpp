#include "format.hpp"

namespace
{
   size_t sample_size(snd_pcm_format_t format)
   {
      switch (format)
      {
      case SND_PCM_FORMAT_U8:
      case SND_PCM_FORMAT_S8:
         return 1;
      case SND_PCM_FORMAT_U16_LE:
      case SND_PCM_FORMAT_S16_LE:
      case SND_PCM_FORMAT_U16_BE:
      case SND_PCM_FORMAT_S16_BE:
         return 2;
      case SND_PCM_FORMAT_U24_LE:
      case SND_PCM_FORMAT_S24_LE:
      case SND_PCM_FORMAT_U24_BE:
      case SND_PCM_FORMAT_S24_BE:
         return 3;
      case SND_PCM_FORMAT_U32_LE:
      case SND_PCM_FORMAT_S32_LE:
      case SND_PCM_FORMAT_U32_BE:
      case SND_PCM_FORMAT_S32_BE:
         return 4;
      default:
         assert(false);
      }
   }
}

format::format()
   : sample_rate()
   , channels()
   , fmt(SND_PCM_FORMAT_UNKNOWN)
{}

format::format(unsigned sample_rate, unsigned channels, snd_pcm_format_t fmt)
   : sample_rate(sample_rate)
   , channels(channels)
   , fmt(fmt)
{}

size_t format::frame_size() const
{
   return sample_size(fmt) * channels;
}