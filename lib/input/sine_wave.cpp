#include "input_stream.h"
#include <cmath>
#include <limits>
#include <stdexcept>
#include <boost/cstdint.hpp>
#include <boost/make_shared.hpp>

namespace
{
   template <typename T>
   struct sine_wave_gen : input_stream
   {
      sine_wave_gen(format const& fmt, double freq)
         : fmt_(fmt)
         , freq_(freq)
         , current_pos_()
      {}

      format get_format()
      {
         return fmt_;
      }

      boost::optional<size_t> get_size()
      {
         return boost::none;
      }

      void set_position(size_t frame_n)
      {
         current_pos_ = frame_n;
      }

      size_t get_position()
      {
         return current_pos_;
      }

      size_t get_available()
      {
         return std::numeric_limits<size_t>::max();
      }

      void read(void* buf, size_t size)
      {
         T* out = static_cast<T*>(buf);
         for (size_t i = 0; i != size; ++i)
         {
            T val = T(std::numeric_limits<T>::max() * sin(freq_ * (double)current_pos_++ / (double)fmt_.sample_rate * 2 * M_PI));
            for (size_t c = 0; c != fmt_.channels; ++c)
                  *out++ = val;
         }
      }

   private:
      format fmt_;
      double freq_;
      size_t current_pos_;
   };
}

input_stream_sp open_sine_generator(input_stream::format const& fmt, double freq)
{
   switch (fmt.fmt)
   {
   case SND_PCM_FORMAT_S8:
      return boost::make_shared<sine_wave_gen<boost::int8_t> >(fmt, freq);
   case SND_PCM_FORMAT_U8:
      return boost::make_shared<sine_wave_gen<boost::uint8_t> >(fmt, freq);
   case SND_PCM_FORMAT_S16:
      return boost::make_shared<sine_wave_gen<boost::int16_t> >(fmt, freq);
   case SND_PCM_FORMAT_U16:
      return boost::make_shared<sine_wave_gen<boost::uint16_t> >(fmt, freq);
   default:
      throw std::runtime_error("unknown sine wave format");
   }
}
