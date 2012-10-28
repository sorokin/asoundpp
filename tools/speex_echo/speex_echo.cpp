#include "asoundpp.hpp"
#include "input_device.hpp"
#include "output_device.hpp"

#include <speex/speex.h>
#include <sstream>

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

enum speex_profile
{
    narrowband_speex_profile,
    wideband_speex_profile
};

struct speex_encoder : boost::noncopyable
{
    speex_encoder(speex_profile p = wideband_speex_profile,
                  int quality = 10)               // integer [0..10]
    {
        speex_bits_init(&bits_);
        encoder_state_ = speex_encoder_init(p == wideband_speex_profile ? &speex_wb_mode : &speex_nb_mode);

        int fs;
        do_encoder_ctl(encoder_state_, SPEEX_GET_FRAME_SIZE, &fs);
        frame_size_ = fs;

        do_encoder_ctl(encoder_state_, SPEEX_SET_QUALITY, &quality);
    }

    size_t frame_size()
    {
        return frame_size_;
    }

    void encode(void const* data)
    {
        speex_bits_reset(&bits_);
        speex_encode_int(encoder_state_, const_cast<spx_int16_t*>(static_cast<spx_int16_t const*>(data)), &bits_);

        int outsize = speex_bits_nbytes(&bits_);
        if (outsize > buf_.size())
            buf_.resize(outsize);

        int outsize2 = speex_bits_write(&bits_, &buf_[0], outsize);
        assert(outsize == outsize2);
    }

    void const* get_encoded_data()
    {
        return &buf_[0];
    }

    size_t get_encoded_size()
    {
        return buf_.size();
    }

    ~speex_encoder()
    {
        speex_encoder_destroy(encoder_state_);
        speex_bits_destroy(&bits_);
    }

private:
    SpeexBits bits_;
    void* encoder_state_;
    size_t frame_size_;
    std::vector<char> buf_;

};

struct speex_decoder : boost::noncopyable
{
    speex_decoder(speex_profile p = wideband_speex_profile)
    {
        speex_bits_init(&bits_);
        decoder_state_ = speex_decoder_init(p == wideband_speex_profile ? &speex_wb_mode : &speex_nb_mode);

        int fs;
        do_decoder_ctl(decoder_state_, SPEEX_GET_FRAME_SIZE, &fs);
        frame_size_ = fs;
    }

    size_t frame_size()
    {
        return frame_size_;
    }

    void decode(void const* data, size_t size, void* output)
    {
        speex_bits_read_from(&bits_, const_cast<char*>(static_cast<char const*>(data)), size);
        speex_decode_int(decoder_state_, &bits_, static_cast<spx_int16_t*>(output));
    }

    ~speex_decoder()
    {
        speex_decoder_destroy(decoder_state_);
        speex_bits_destroy(&bits_);
    }

private:
    SpeexBits bits_;
    void* decoder_state_;
    size_t frame_size_;

};

int main()
{
    asound::global_config_cleanup cleanup;
    input_device id("default", format(44100, 1, SND_PCM_FORMAT_S16));
    output_device od("default", format(44100, 1, SND_PCM_FORMAT_S16));

    speex_encoder se;
    speex_decoder de;

    std::vector<char> v(se.frame_size() * id.get_format().frame_size());
    std::vector<char> vv(de.frame_size() * id.get_format().frame_size());
    for (;;)
    {
        id.read(&v[0], se.frame_size());

        se.encode(&v[0]);
        de.decode(se.get_encoded_data(), se.get_encoded_size(), &vv[0]);

        od.write(&vv[0], de.frame_size());
    }
}
