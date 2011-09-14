#include <iostream>
#include <vector>
#include <stdexcept>

#include <boost/bind.hpp>
#include <boost/optional.hpp>
#include <boost/make_shared.hpp>

#include <boost/iostreams/device/mapped_file.hpp>

#include "input_stream.h"

namespace
{

struct chunk_header
{
   boost::uint32_t signature;
   boost::uint32_t size;
};

struct riff_header
{
   chunk_header hdr;
   boost::uint32_t format;
};

struct fmt_chunk
{
   chunk_header hdr;
   boost::uint16_t format;
   boost::uint16_t channels;
   boost::uint32_t sample_rate;
   boost::uint32_t byte_rate;
   boost::uint16_t block_align;
   boost::uint16_t bits_per_sample;

   size_t sample_size() const
   {
      assert((bits_per_sample % 8) == 0);
      return bits_per_sample / 8;
   }

   size_t frame_size() const
   {
      return sample_size() * channels;
   }
};

template <typename T>
T const& read_chunk(boost::iostreams::mapped_file_source const& mapping, size_t offset)
{
   size_t size = mapping.size();
   size_t end = offset + sizeof(T);

   if (end > size)
      throw std::runtime_error("unable to read chunk"); // TODO: more verbose

   return *reinterpret_cast<T const*>(mapping.data() + offset);
}

void verify_riff_header(riff_header const& riff, size_t actual_file_size)
{
   if (riff.hdr.signature != 0x46464952)
      throw std::runtime_error("invalid riff signature");

   if (riff.hdr.size != (actual_file_size - sizeof(chunk_header)))
      throw std::runtime_error("mismatched size of file and size in riff header");

   if (riff.format != 0x45564157)
      throw std::runtime_error("invalid format in riff header");
}

void verify_fmt_chunk(fmt_chunk const& fmt)
{
   if (fmt.hdr.signature != 0x20746d66)
      throw std::runtime_error("invalid fmt header signature");

   if (fmt.hdr.size != sizeof(fmt_chunk) - sizeof(chunk_header))
      throw std::runtime_error("mismatched size of fmt header");

   if ((fmt.bits_per_sample % 8) != 0)
      throw std::runtime_error("bits_per_sample is not in multiples of 8");

   if (fmt.format != 1)
      throw std::runtime_error("unknown wave format");
}

void verify_data_chunk_header(chunk_header const& data_hdr, fmt_chunk const& fmt, size_t offset, size_t actual_file_size)
{
   if (data_hdr.signature != 0x61746164)
      throw std::runtime_error("invalid data header signature");

   if (actual_file_size != offset + sizeof(chunk_header) + data_hdr.size)
      throw std::runtime_error("mismatched size of file and size in data header");

   if ((data_hdr.size % fmt.frame_size()) != 0)
      throw std::runtime_error("data chunk size is not in multiples of frame size");
}

struct wave_file : input_stream
{
   explicit wave_file(std::string const& filename)
      : mapping(filename)
      , current_frame()
   {
      riff_header riff = read_chunk<riff_header>(mapping, 0);
      verify_riff_header(riff, mapping.size());

      fmt_chunk fmt = read_chunk<fmt_chunk>(mapping, sizeof(riff_header));
      verify_fmt_chunk(fmt);

      size_t data_hdr_offset = sizeof(riff_header) + sizeof(fmt_chunk);
      chunk_header data_hdr = read_chunk<chunk_header>(mapping, data_hdr_offset);
      verify_data_chunk_header(data_hdr, fmt, data_hdr_offset, mapping.size());

      myformat.sample_rate = fmt.sample_rate;
      myformat.channels    = fmt.channels;
      myformat.sample_size = fmt.bits_per_sample / 8;

      number_of_frames_ = data_hdr.size / myformat.frame_size();
   }

   char const* data() const
   {
      return mapping.data() + sizeof(riff_header) + sizeof(fmt_chunk) + sizeof(chunk_header);
   }

   size_t number_of_frames()
   {
      return number_of_frames_;
   }

   format get_format()
   {
      return myformat;
   }

   void seek(size_t frame_n)
   {
      current_frame = frame_n;
   }

   size_t get_position()
   {
      return current_frame;
   }

   void read(void* buf, size_t n)
   {
      assert((current_frame + n) <= number_of_frames_);
      memcpy(buf, data() + current_frame * myformat.frame_size(), n * myformat.frame_size());
      current_frame += n;
   }

private:
   boost::iostreams::mapped_file_source mapping;

   input_stream::format myformat;
   size_t               number_of_frames_;
   size_t               current_frame;
};

}

input_stream_sp open_wave_file(std::string const& filename)
{
   return boost::make_shared<wave_file>(filename);
}
