#include "asoundpp.hpp"
#include "output_device.hpp"
#include "input_stream.h"

int main()
{
    asound::global_config_cleanup cleanup;
    input_stream_sp is = open_input_device(input_stream::format(44100, 1, SND_PCM_FORMAT_S16), "default");
    output_device od("default", input_stream::format(44100, 1, SND_PCM_FORMAT_S16));

    std::vector<char> v(1000 * is->get_format().frame_size());
    for (;;)
    {
        size_t nn = is->get_available();
        if (nn == 0)
            nn = 1;
        else if (nn > 1000)
            nn = 1000;
        is->read(&v[0], nn);
        od.write(&v[0], nn);
    }
}
