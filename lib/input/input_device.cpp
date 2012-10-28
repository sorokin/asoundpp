#include "input_stream.h"
#include "asoundpp.hpp"
#include <boost/make_shared.hpp>

namespace
{
    struct input_device : input_stream
    {
        input_device(input_stream::format const& fmt, std::string const& device_name)
            : fmt_(fmt)
            , d_(device_name.c_str(), SND_PCM_STREAM_CAPTURE, 0)
        {
            d_.set_params(fmt.fmt, SND_PCM_ACCESS_RW_INTERLEAVED, fmt.channels, fmt.sample_rate, true, 500000);
        }

        format get_format()
        {
            return fmt_;
        }

        boost::optional<size_t> get_size()
        {
            return boost::none;
        }

        void set_position(size_t frame_n)
        {
            assert(false); // not applicable
        }

        size_t get_position()
        {
            assert(false); // not applicable
        }

        size_t get_available()
        {
            return d_.avail_update();
        }

        void read(void* buf, size_t size)
        {
            try
            {
                d_.readi(buf, size);
            }
            catch (asound::pcm::underrun const&)
            {
                d_.prepare();
            }
        }

    private:
        input_stream::format fmt_;
        asound::pcm::device d_;
    };
}

input_stream_sp open_input_device(input_stream::format const& fmt, std::string const& device_name)
{
    return boost::make_shared<input_device>(fmt, device_name);
}
