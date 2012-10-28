#include "speex_decoder.hpp"
#include <sstream>
#include <stdexcept>

namespace
{
    void do_decoder_ctl(void* decoder_state, int request, void* data)
    {
        int r = speex_decoder_ctl(decoder_state, request, data);
        if (r != 0)
        {
            std::stringstream ss;
            ss << "speex_decoder_ctl failed (error: " << r << ", request code: " << request << ")";
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

speex_decoder::speex_decoder(speex_profile p)
{
    speex_bits_init(&bits_);
    decoder_state_ = speex_decoder_init(p == wideband_speex_profile ? &speex_wb_mode : &speex_nb_mode);

    int fs;
    do_decoder_ctl(decoder_state_, SPEEX_GET_FRAME_SIZE, &fs);
    block_size_ = fs;
}

speex_decoder::~speex_decoder()
{
    speex_decoder_destroy(decoder_state_);
    speex_bits_destroy(&bits_);
}

size_t speex_decoder::block_size()
{
    return block_size_;
}

void speex_decoder::decode(void const* data, size_t size, void* output)
{
    speex_bits_read_from(&bits_, const_cast<char*>(static_cast<char const*>(data)), size);
    speex_decode_int(decoder_state_, &bits_, static_cast<spx_int16_t*>(output));
}
