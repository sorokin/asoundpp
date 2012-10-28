#include "asoundpp.hpp"
#include "input_device.hpp"
#include "output_device.hpp"
#include "speex_encoder.hpp"
#include "speex_decoder.hpp"

#include <speex/speex.h>

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
