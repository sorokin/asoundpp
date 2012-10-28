#include "asoundpp.hpp"
#include "input_device.hpp"
#include "output_device.hpp"
#include "speex_encoder.hpp"
#include "speex_decoder.hpp"

#include <speex/speex.h>

int main()
{
    asound::global_config_cleanup cleanup;

    format fmt(44100, 1, SND_PCM_FORMAT_S16);
    input_device in("default", fmt);
    output_device out("default", fmt);

    speex_encoder enc;
    speex_decoder dec;

    std::vector<char> v(enc.frame_size() * fmt.frame_size());
    std::vector<char> vv(dec.frame_size() * fmt.frame_size());

    for (;;)
    {
        in.read(&v[0], enc.frame_size());

        enc.encode(&v[0]);
        dec.decode(enc.get_encoded_data(), enc.get_encoded_size(), &vv[0]);

        out.write(&vv[0], dec.frame_size());
    }
}
