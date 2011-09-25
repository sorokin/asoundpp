#ifndef INPUT_STREAM_H
#define INPUT_STREAM_H

#include <cstdlib>
#include <boost/shared_ptr.hpp>
#include <alsa/asoundlib.h>

struct input_stream
{
   struct format
   {
      unsigned sample_rate;
      unsigned channels;
      snd_pcm_format_t fmt;

      size_t frame_size() const;
   };

   virtual ~input_stream() = 0;

   virtual format get_format() = 0;
   virtual size_t get_size() = 0;

   virtual void seek(size_t frame_n) = 0;
   virtual size_t get_position() = 0;

   virtual void read(void* buf, size_t get_size) = 0; // read get_size * get_format().frame_size() bytes
};

typedef boost::shared_ptr<input_stream> input_stream_sp;

input_stream_sp open_wave_file(std::string const& filename);
input_stream_sp open_flac_file(std::string const& filename);

#endif // INPUT_STREAM_H
