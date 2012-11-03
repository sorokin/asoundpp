#include "asoundpp.hpp"
#include "input_device.hpp"
#include "output_device.hpp"

int main()
{
    asound::global_config_cleanup cleanup;

    frame_format fmt(44100, 1, SOUNDIO_SAMPLE_FORMAT_S16);
    input_device in("default", fmt);
    output_device out("default", fmt);

    std::vector<char> v(1000 * in.get_format().frame_size());
    for (;;)
    {
        size_t nn = in.get_available();
        if (nn == 0)
            nn = 1;
        else if (nn > 1000)
            nn = 1000;
        in.read(&v[0], nn);
        out.write(&v[0], nn);
    }
}

