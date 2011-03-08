#include "asoundpp.hpp"

int main()
{
   asound::global_config_cleanup cfg_cleanup;

   snd_config_update();
}
