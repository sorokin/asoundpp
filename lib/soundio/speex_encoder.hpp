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

    // speex encodes data by blocks of fixed size
    // this function returns the size of one block in shorts
    size_t block_size();

    // data must contain at least block_size() * sizeof(short) bytes
    // to avoid extra memory allocations encode() stores result inside the object
    // after calling encode() you can get encoded data by calling get_encoded_data()/get_encoded_size()
    // values returned by get_encoded_data()/get_encoded_size() are valid until next invokation of encode()
    // it is an error to call get_encoded_data()/get_encoded_size() after constuction of speex_encoder before encode()
    void encode(void const* data);
    void const* get_encoded_data();
    size_t get_encoded_size();

private:
    SpeexBits bits_;
    void* encoder_state_;
    size_t block_size_;
    std::vector<char> buf_;
};

#endif
