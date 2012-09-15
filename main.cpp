#include <stdio.h>
#include <stdlib.h>
#include "FLAC/stream_decoder.h"

#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>

#include "input_stream.h"
#include "wav_writer.hpp"

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
            fprintf(stderr, "usage: %s infile.flac outfile.wav\n", argv[0]);
            return 1;
    }

    input_stream_sp in = open_flac_file(argv[1]);

    wav_writer out(argv[2], in->get_format());

    std::vector<char> buf(8192 * in->get_format().frame_size());
    size_t number_of_blocks = *in->get_size() / 8192;
    size_t last_block_size = *in->get_size() % 8192;

    for (size_t i = 0; i != number_of_blocks; ++i)
    {
       in->read(&buf[0], 8192);
       out.write(&buf[0], 8192);
    }

    if (last_block_size != 0)
    {
        in->read(&buf[0], last_block_size);
        out.write(&buf[0], last_block_size);
    }

    return 0;
}
