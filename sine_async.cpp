#include "asound_async.hpp"
#include "sine_wave_generator.hpp"

#include <iostream>
#include <vector>

#include <boost/bind.hpp>
#include <boost/optional.hpp>

unsigned int const RATE = 44100;

void fill_buffer(std::vector<short>& buf, sine_wave_generator& g, size_t size)
{
   size_t current_size = buf.size();
   if (current_size >= size)
      return;

   buf.resize(size);

   for (size_t i = current_size; i != size; ++i)
      buf[i] = g();
}

struct pcm_test
{
   pcm_test(asio::io_service& io_service,
            asound::pcm::device& d)
      : ss(io_service, SIGINT, SIGTERM, SIGQUIT)
      , ad(boost::in_place(boost::ref(io_service),
                           boost::ref(d),
                           boost::bind(&pcm_test::do_write, this),
                           boost::bind(&pcm_test::do_error, this)))
      , wave_gen(30000, 440., RATE)
   {
      ss.async_wait(boost::bind(&pcm_test::on_quit_signal, this));
   }

private:
   void do_write()
   {
      assert(ad);

      size_t const desired_buffer_size = 1000;

      fill_buffer(buffer, wave_gen, desired_buffer_size);
      size_t written = ad->write(&buffer[0], buffer.size());
      buffer.erase(buffer.begin(), buffer.begin() + written);
   }

   void do_error()
   {
      ad = boost::none;
   }

   void on_quit_signal()
   {
      std::cout << "Have a nice day!" << std::endl;
      ad = boost::none;
   }

private:
   asio::signal_set ss;
   boost::optional<asound::pcm::async_device> ad;
   sine_wave_generator wave_gen;
   std::vector<short> buffer;
};

int main(int , char *[])
{
   try
   {
      asio::io_service io_service;

      asound::pcm::device d("default", SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);

      d.set_params(SND_PCM_FORMAT_S16,
                   SND_PCM_ACCESS_RW_INTERLEAVED,
                   1,
                   RATE,
                   true,
                   500000);

      pcm_test pcm_test(io_service, d);

      io_service.run();
   }
   catch (std::exception const& e)
   {
      std::cout << "exception: " << e.what() << std::endl;
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}
