#include "speex_encoder.hpp"
#include <sstream>
#include <stdexcept>
#include <cassert>

namespace
{
    void do_encoder_ctl(void* encoder_state, int request, void* data)
    {
        int r = speex_encoder_ctl(encoder_state, request, data);
        if (r != 0)
        {
            std::stringstream ss;
            ss << "speex_encoder_ctl failed (error: " << r << ", request code: " << request << ")";
            switch (r)
            {
            case -1:
                ss << " unknown request";
            case -2:
                ss << " invalid parameter";
            }

            throw std::runtime_error(ss.str());
        }
    }
}

speex_encoder::speex_encoder(speex_profile p, int quality)
{
    speex_bits_init(&bits_);
    encoder_state_ = speex_encoder_init(p == wideband_speex_profile ? &speex_wb_mode : &speex_nb_mode);

    int fs;
    do_encoder_ctl(encoder_state_, SPEEX_GET_FRAME_SIZE, &fs);
    block_size_ = fs;

    do_encoder_ctl(encoder_state_, SPEEX_SET_QUALITY, &quality);
}

speex_encoder::~speex_encoder()
{
    speex_encoder_destroy(encoder_state_);
    speex_bits_destroy(&bits_);
}

size_t speex_encoder::block_size()
{
    return block_size_;
}

void speex_encoder::encode(void const* data)
{
    speex_bits_reset(&bits_);
    speex_encode_int(encoder_state_, const_cast<spx_int16_t*>(static_cast<spx_int16_t const*>(data)), &bits_);

    int outsize = speex_bits_nbytes(&bits_);
    if (outsize > buf_.size())
        buf_.resize(outsize);

    int outsize2 = speex_bits_write(&bits_, &buf_[0], outsize);
    assert(outsize == outsize2);
}

void const* speex_encoder::get_encoded_data()
{
    return &buf_[0];
}

size_t speex_encoder::get_encoded_size()
{
    return buf_.size();
}
