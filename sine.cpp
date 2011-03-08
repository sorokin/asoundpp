#include "asoundpp.hpp"
#include <math.h>

#include <vector>
#include <iostream>

short generate_harmonic_sample(short amplitude, double frequency, unsigned sample_rate, unsigned sample_num)
{
   return amplitude * sin(frequency * (double)sample_num / (double)sample_rate * 2 * M_PI);
}

struct frame
{
   short ch0;
   short ch1;
} __attribute__((packed));

int main()
{
   asound::global_config_cleanup cfg_cleanup;
   asound::pcm::device d("default", SND_PCM_STREAM_PLAYBACK);

   unsigned const rate   = 44100;
   size_t   const frames = 8192;

   d.set_params(SND_PCM_FORMAT_S16_LE,
                SND_PCM_ACCESS_RW_INTERLEAVED,
                2,
                rate,
                true,
                500000);

   std::vector<frame> data(frames);

   unsigned current_sample = 0;
   for (unsigned l1 = 0; l1 < 30; l1++) {
      for (size_t l2 = 0; l2 < frames; l2++) {
         data[l2].ch0 = generate_harmonic_sample(30000, 220., rate, current_sample);
         data[l2].ch1 = generate_harmonic_sample(30000, 440., rate, current_sample);
         ++current_sample;
      }

      try
      {
         d.writei(&data[0], frames);
      }
      catch (asound::pcm::underrun const&)
      {
         std::cout << "underrun!\n";
         d.prepare();
      }
   }

   return 0;
}
