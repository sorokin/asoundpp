#ifndef INPUT_STREAM_H
#define INPUT_STREAM_H

#include <cstdlib>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <alsa/asoundlib.h>

struct input_stream
{
   struct format
   {
      format();
      format(unsigned sample_rate, unsigned channels, snd_pcm_format_t fmt);
      size_t frame_size() const;

      unsigned sample_rate;
      unsigned channels;
      snd_pcm_format_t fmt;
   };

   virtual ~input_stream() = 0;

   virtual format get_format() = 0;
   virtual boost::optional<size_t> get_size() = 0;

   virtual void set_position(size_t frame_n) = 0;
   virtual size_t get_position() = 0;

   virtual size_t get_available() = 0;

   virtual void read(void* buf, size_t size) = 0; // read size * get_format().frame_size() bytes
};

typedef boost::shared_ptr<input_stream> input_stream_sp;

void seek_backward(input_stream& stream, size_t frame_n);
void seek_forward(input_stream& stream, size_t frame_n);

input_stream_sp open_wave_file(std::string const& filename);
input_stream_sp open_flac_file(std::string const& filename);
input_stream_sp open_sine_generator(input_stream::format const& fmt, double freq);

#endif // INPUT_STREAM_H
