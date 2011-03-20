#include "asoundpp.hpp"
#include "sine_wave_generator.hpp"

#include <vector>
#include <iostream>

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

      sine_wave_generator ch0_generator(30000, 220., rate);
      sine_wave_generator ch1_generator(30000, 440., rate);

      for (unsigned l1 = 0; l1 < 30; l1++)
      {
         for (size_t l2 = 0; l2 < frames; l2++)
            data[l2] = my_frame_type(my_frame_type::value_type(ch0_generator()),
                                     my_frame_type::value_type(ch1_generator()));

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
