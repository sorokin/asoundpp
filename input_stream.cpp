#include "input_stream.h"

size_t input_stream::format::frame_size() const
{
   return sample_size * channels;
}

input_stream::~input_stream()
{
}
