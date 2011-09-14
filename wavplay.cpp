//#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include "asound_async.hpp"

#include "wave.h"

#include <iostream>
#include <vector>

#include <boost/bind.hpp>
#include <boost/optional.hpp>

struct pcm_test
{
   typedef boost::function<void (const boost::system::error_code&, int)> do_signal_t;

   pcm_test(boost::asio::io_service& io_service,
            asound::pcm::device& d,
            wave_file_mapping const& mapping)
      : ss(io_service, SIGINT, SIGTERM, SIGQUIT)
      , ad(boost::in_place(boost::ref(io_service),
                           boost::ref(d),
                           boost::bind(&pcm_test::do_write, this),
                           boost::bind(&pcm_test::close_device, this)))
      , mapping(mapping)
      , current_sample(0)
   {
      do_signal_t ds = boost::bind(&pcm_test::close_device, this);
      ss.async_wait(oc.wrap(ds));
   }

private:
   void do_write()
   {
      assert(ad);

      size_t frame_size = mapping.format().sample_size() * mapping.format().channels;
      size_t number_of_frames = mapping.size() / frame_size;
      void const* data_offset = static_cast<char const*>(mapping.data()) + current_sample * frame_size;

      if (number_of_frames == current_sample)
      {
         // how to drain device asynchonously?
         close_device();
         return;
      }

      size_t number_of_frames_to_write = std::min(number_of_frames - current_sample, ad->avail_update());

      size_t written = ad->write(data_offset, number_of_frames_to_write);
      current_sample += written;
   }

   void do_signal(const boost::system::error_code&, int /*sig_num*/)
   {
      ad = boost::none;
   }

   void close_device()
   {
      ss.cancel();
      ad = boost::none;
   }

private:
   boost::asio::signal_set ss;
   boost::optional<asound::pcm::async_device> ad;
   wave_file_mapping const& mapping;
   size_t current_sample;
   operation_cancelation oc;
};

int main(int , char *[])
{
   try
   {
      wave_file_mapping mapping("/home/ivan/d/alsa/asoundpp/1.wav");

      std::cout << "bits per sample: " << mapping.format().bits_per_sample << std::endl;
      std::cout << "channels:        " << mapping.format().channels        << std::endl;
      std::cout << "sample rate:     " << mapping.format().sample_rate     << std::endl;

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
      std::cout << "Have a nice day!" << std::endl;
   }
   catch (std::exception const& e)
   {
      std::cout << "exception: " << e.what() << std::endl;
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}
