#include "input_stream.h"

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

size_t input_stream::format::frame_size() const
{
   return sample_size(fmt) * channels;
}

input_stream::~input_stream()
{
}

void seek_backward(input_stream& stream, size_t frame_n)
{
   size_t p = stream.get_position();
   if (p < frame_n)
      p = 0;
   else
      p -= frame_n;
   stream.set_position(p);
}

namespace
{
   size_t get_limit(input_stream& stream)
   {
      boost::optional<size_t> size = stream.get_size();
      if (size)
         return *size;
      return stream.get_available();
   }
}

void seek_forward(input_stream& stream, size_t frame_n)
{
   size_t p = stream.get_position();
   size_t limit = get_limit(stream);
   if ((limit - p) < frame_n)
      p = limit;
   else
      p += frame_n;
   stream.set_position(p);
}
