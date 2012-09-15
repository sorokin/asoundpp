#include <iostream>

#include "input_stream.h"
#include "asoundpp.hpp"

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "usage: %s infile.wav\n", argv[0]);
        return 1;
    }

    input_stream_sp stream = open_wave_file(argv[1]);

    std::cerr << "format:       " << snd_pcm_format_name(stream->get_format().fmt)
              << " (" << snd_pcm_format_description(stream->get_format().fmt) << ")" <<std::endl;
    std::cerr << "channels:     " << stream->get_format().channels        << std::endl;
    std::cerr << "sample rate:  " << stream->get_format().sample_rate     << std::endl;

    asound::global_config_cleanup cfg_cleanup;

    asound::pcm::device d("default", SND_PCM_STREAM_PLAYBACK, 0);

    d.set_params(stream->get_format().fmt,
                 SND_PCM_ACCESS_RW_INTERLEAVED,
                 stream->get_format().channels,
                 stream->get_format().sample_rate,
                 true,
                 500000);

    std::vector<char> buf(8192 * stream->get_format().frame_size());
    size_t number_of_blocks = *stream->get_size() / 8192;
    size_t last_block_size = *stream->get_size() % 8192;

    for (size_t i = 0; i != number_of_blocks; ++i)
    {
       stream->read(&buf[0], 8192);
       d.writei(&buf[0], 8192);
    }

    if (last_block_size != 0)
    {
        stream->read(&buf[0], last_block_size);
        d.writei(&buf[0], last_block_size);
    }
}
