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

    size_t frame_size();

    void decode(void const* data, size_t size, void* output);


private:
    SpeexBits bits_;
    void* decoder_state_;
    size_t frame_size_;
};

#endif
