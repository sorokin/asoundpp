#include "asoundpp.hpp"
#include "input_device.hpp"
#include "output_device.hpp"

int main()
{
    asound::global_config_cleanup cleanup;
    input_device id(format(44100, 1, SND_PCM_FORMAT_S16), "default");
    output_device od("default", format(44100, 1, SND_PCM_FORMAT_S16));

    std::vector<char> v(1000 * id.get_format().frame_size());
    for (;;)
    {
        size_t nn = id.get_available();
        if (nn == 0)
            nn = 1;
        else if (nn > 1000)
            nn = 1000;
        id.read(&v[0], nn);
        od.write(&v[0], nn);
    }
}
