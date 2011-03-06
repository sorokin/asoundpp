#include "asoundpp.hpp"

#include <iostream>

int main()
{
   asound::global_config_cleanup cleanup;
   asound::pcm::device d("default", SND_PCM_STREAM_PLAYBACK);
   asound::pcm::info i = d.get_info();

   std::cout << "device: "    << i.get_device() << "\n";
   std::cout << "subdevice: " << i.get_subdevice() << "\n";
   std::cout << "stream: "    << snd_pcm_stream_name(i.get_stream()) << "\n";
   std::cout << "card: "      << i.get_card() << "\n";
   std::cout << "id: "        << i.get_id() << "\n";

   return 0;
}
