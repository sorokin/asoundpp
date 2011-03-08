#include "asoundpp.hpp"
#include <math.h>

#include <vector>
#include <iostream>

short generate_harmonic_sample(short amplitude, double frequency, unsigned sample_rate, unsigned sample_num)
{
   return amplitude * sin(frequency * (double)sample_num / (double)sample_rate * 2 * M_PI);
}

int main()
{
   try
   {
      asound::global_config_cleanup cfg_cleanup;
      asound::pcm::device d("default", SND_PCM_STREAM_PLAYBACK);

      typedef frame2_s16_le my_frame_type;

      unsigned const rate   = 44100;
      size_t   const frames = 8192;

      d.set_params(my_frame_type::format,
                   SND_PCM_ACCESS_RW_INTERLEAVED,
                   my_frame_type::channels,
                   rate,
                   true,
                   500000);

      std::vector<my_frame_type> data(frames);

      unsigned current_frame = 0;
      for (unsigned l1 = 0; l1 < 30; l1++)
      {
         for (size_t l2 = 0; l2 < frames; l2++)
         {
            data[l2].ch[0] = generate_harmonic_sample(30000, 220., rate, current_frame);
            data[l2].ch[1] = generate_harmonic_sample(30000, 440., rate, current_frame);
            ++current_frame;
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
   }
   catch (std::runtime_error const& e)
   {
      std::cerr << "error: " << e.what() << "\n";
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}
