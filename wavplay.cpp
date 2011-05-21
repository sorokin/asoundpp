#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include "asound_async.hpp"

#include "wave.h"

#include <iostream>
#include <vector>

#include <boost/bind.hpp>
#include <boost/optional.hpp>

struct pcm_test
{
   pcm_test(boost::asio::io_service& io_service,
            asound::pcm::device& d,
            wave_file_mapping const& mapping)
      : ss(io_service, SIGINT, SIGTERM, SIGQUIT)
      , ad(boost::in_place(boost::ref(io_service),
                           boost::ref(d),
                           boost::bind(&pcm_test::do_write, this),
                           boost::bind(&pcm_test::do_error, this)))
      , mapping(mapping)
      , current_sample(0)
   {
      ss.async_wait(boost::bind(&pcm_test::on_quit_signal, this));
   }

private:
   void do_write()
   {
      assert(ad);

      size_t sample_size = 2 * mapping.format().channels;
      size_t number_of_samples = mapping.size() / sample_size;
      void const* data_offset = static_cast<char const*>(mapping.data()) + current_sample * sample_size;

      if (number_of_samples == current_sample)
      {
         // how to drain device asynchonously?
         ad = boost::none;
         return;
      }

      size_t number_of_samples_to_write = std::min(number_of_samples - current_sample, ad->avail_update());

      size_t written = ad->write(data_offset, number_of_samples_to_write);
      current_sample += written;

      std::cout << number_of_samples_to_write << " " << written << std::endl;
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
   boost::asio::signal_set ss;
   boost::optional<asound::pcm::async_device> ad;
   wave_file_mapping const& mapping;
   size_t current_sample;
};

int main(int , char *[])
{
   try
   {
      wave_file_mapping mapping("/home/ivan/d/alsa/asoundpp/1.wav");

      if (mapping.format().format != 1)
         throw std::runtime_error("unknown wave format");
      if (mapping.format().bits_per_sample != 16)
         throw std::runtime_error("only 16bit wave file are supported");
      if ((mapping.size() % (2 * mapping.format().channels)) != 0)
         throw std::runtime_error("invalid data size");

      asound::global_config_cleanup cfg_cleanup;
      boost::asio::io_service io_service;

      asound::pcm::device d("default", SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);

      d.set_params(SND_PCM_FORMAT_S16,
                   SND_PCM_ACCESS_RW_INTERLEAVED,
                   mapping.format().channels,
                   mapping.format().sample_rate,
                   true,
                   500000);

      pcm_test pcm_test(io_service, d, mapping);

      io_service.run();
   }
   catch (std::exception const& e)
   {
      std::cout << "exception: " << e.what() << std::endl;
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}
