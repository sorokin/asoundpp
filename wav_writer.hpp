#ifndef WAV_WRITER_HPP
#define WAV_WRITER_HPP

#include "input_stream.h"
#include <boost/noncopyable.hpp>

struct wav_writer : boost::noncopyable
{
    wav_writer(char const* filename, input_stream::format fmt);
    ~wav_writer();

    void write(void const* buf, size_t number_of_frames);

private:
    FILE* handle_;
    input_stream::format fmt_;
    size_t number_of_frames_;
};

#endif // WAV_WRITER_HPP
