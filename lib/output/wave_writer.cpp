#include "wave_writer.hpp"
#include <boost/cstdint.hpp>
#include <sstream>
#include <stdexcept>

namespace
{
    bool write_little_endian_uint16(FILE *f, boost::uint16_t x)
    {
            return
                    fputc(x, f) != EOF &&
                    fputc(x >> 8, f) != EOF
            ;
    }

    bool write_little_endian_uint32(FILE *f, boost::uint32_t x)
    {
            return
                    fputc(x, f) != EOF &&
                    fputc(x >> 8, f) != EOF &&
                    fputc(x >> 16, f) != EOF &&
                    fputc(x >> 24, f) != EOF
            ;
    }

    bool write_wave_header(FILE* fout, input_stream::format fmt, size_t number_of_frames)
    {
        boost::uint32_t total_size = (boost::uint32_t)(number_of_frames * fmt.frame_size());

        return
                fwrite("RIFF", 1, 4, fout) < 4 ||
                !write_little_endian_uint32(fout, total_size + 36) ||
                fwrite("WAVEfmt ", 1, 8, fout) < 8 ||
                !write_little_endian_uint32(fout, 16) ||
                !write_little_endian_uint16(fout, 1) ||
                !write_little_endian_uint16(fout, (boost::uint16_t)fmt.channels) ||
                !write_little_endian_uint32(fout, fmt.sample_rate) ||
                !write_little_endian_uint32(fout, fmt.sample_rate * fmt.frame_size()) ||
                !write_little_endian_uint16(fout, (boost::uint16_t)fmt.frame_size()) || /* block align */
                !write_little_endian_uint16(fout, (boost::uint16_t)fmt.frame_size() / fmt.channels * 8) ||
                fwrite("data", 1, 4, fout) < 4 ||
                !write_little_endian_uint32(fout, total_size);
    }
}

wav_writer::wav_writer(char const* filename, input_stream::format fmt)
    : handle_(fopen(filename, "wb"))
    , fmt_(fmt)
    , number_of_frames_()
{
    if (handle_ == NULL)
    {
        std::stringstream ss;
        ss << "unable to open file for writing \"";
        ss << filename;
        ss << "\"";

        throw std::runtime_error(ss.str());
    }

    write_wave_header(handle_, fmt, 0);
}

wav_writer::~wav_writer()
{
    if (number_of_frames_ != 0)
    {
        fseek(handle_, 0, SEEK_SET);
        write_wave_header(handle_, fmt_, number_of_frames_);
    }

    fclose(handle_);
}

void wav_writer::write(void const* buf, size_t number_of_frames)
{
    fwrite(buf, fmt_.frame_size(), number_of_frames, handle_);
    number_of_frames_ += number_of_frames;
}
