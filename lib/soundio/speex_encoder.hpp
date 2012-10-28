#ifndef SOUNDIO_SPEEX_ENCODER_H
#define SOUNDIO_SPEEX_ENCODER_H

#include "speex_profile.hpp"
#include <boost/noncopyable.hpp>
#include <speex/speex.h>
#include <cstdlib>
#include <vector>

struct speex_encoder : boost::noncopyable
{
    // quality is integer [0..10]
    speex_encoder(speex_profile p = wideband_speex_profile,
                  int quality = 10);
    ~speex_encoder();

    size_t frame_size();

    void encode(void const* data);

    void const* get_encoded_data();
    size_t get_encoded_size();

private:
    SpeexBits bits_;
    void* encoder_state_;
    size_t frame_size_;
    std::vector<char> buf_;
};

#endif
