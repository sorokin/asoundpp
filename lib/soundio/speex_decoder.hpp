#ifndef SOUNDIO_SPEEX_DECODER_H
#define SOUNDIO_SPEEX_DECODER_H

#include "speex_profile.hpp"
#include <boost/noncopyable.hpp>
#include <cstdlib>
#include <speex/speex.h>

struct speex_decoder : boost::noncopyable
{
    speex_decoder(speex_profile p = wideband_speex_profile);
    ~speex_decoder();

    // speex encodes data by blocks of fixed size
    // this function returns the size of one block in shorts
    size_t block_size();

    // output must contain at least block_size() * sizeof(short) bytes
    void decode(void const* data, size_t size, void* output);

private:
    SpeexBits bits_;
    void* decoder_state_;
    size_t block_size_;
};

#endif
