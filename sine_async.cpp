#include "asoundpp.hpp"
#include "sine_wave_generator.hpp"

#include <iostream>

unsigned int const RATE = 44100;

void wait_for_out(asound::pcm::device& d, std::vector<pollfd>& ufds)
{
   for (;;)
   {
      poll(&ufds[0], ufds.size(), -1);

      unsigned short revents = d.revents(ufds);

      if (revents & POLLERR)
         throw std::runtime_error("poll failed");
      if (revents & POLLOUT)
         return;

      std::cout << "*";
   }
}

void fill_buffer(std::vector<short>& buf, sine_wave_generator& g, size_t size)
{
   size_t current_size = buf.size();
   if (current_size >= size)
      return;

   buf.resize(size);

   for (size_t i = current_size; i != size; ++i)
      buf[i] = g();
}

void write_and_poll_loop(asound::pcm::device& d)
{
   std::vector<pollfd> ufds = d.poll_descriptors();

   sine_wave_generator wave_gen(30000, 440., RATE);
   size_t const desired_buffer_size = 1000;
   std::vector<short> buffer;

   for (;;)
   {
      wait_for_out(d, ufds);

      fill_buffer(buffer, wave_gen, desired_buffer_size);
      int err = snd_pcm_writei(d.get(), &buffer[0], buffer.size());
      if (err == -EPIPE)
      {
         throw std::runtime_error("underrun");
      }
      else if (err == -EAGAIN)
      {
         std::cout << ".";
      }
      else if (err < 0)
      {
         throw std::runtime_error("writei failed");
      }
      else
      {
         std::cout << err << std::endl;
         buffer.erase(buffer.begin(), buffer.begin() + err);
      }
   }
}

int main(int argc, char *argv[])
{
   try
   {
      asound::pcm::device d("default", SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);

      d.set_params(SND_PCM_FORMAT_S16,
                   SND_PCM_ACCESS_RW_INTERLEAVED,
                   1,
                   RATE,
                   true,
                   500000);

      write_and_poll_loop(d);
   }
   catch (std::exception const& e)
   {
      std::cout << "exception: " << e.what() << std::endl;
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}
