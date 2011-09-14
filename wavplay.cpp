//#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include "asound_async.hpp"

#include <iostream>
#include <vector>

#include <boost/bind.hpp>
#include <boost/optional.hpp>

#include "input_stream.h"

struct pcm_test
{
   typedef boost::function<void (const boost::system::error_code&, int)> do_signal_t;

   pcm_test(boost::asio::io_service& io_service,
            asound::pcm::device& d,
            input_stream& stream)
      : ss(io_service, SIGINT, SIGTERM, SIGQUIT)
      , ad(boost::in_place(boost::ref(io_service),
                           boost::ref(d),
                           boost::bind(&pcm_test::do_write, this),
                           boost::bind(&pcm_test::close_device, this)))
      , stream(stream)
   {
      do_signal_t ds = boost::bind(&pcm_test::close_device, this);
      ss.async_wait(oc.wrap(ds));
   }

private:
   void do_write()
   {
      assert(ad);

      if (stream.number_of_frames() == stream.get_position())
      {
         // how to drain device asynchonously?
         close_device();
         return;
      }

      size_t number_of_frames_to_write = std::min(stream.number_of_frames() - stream.get_position(), ad->avail_update());
      if (number_of_frames_to_write == 0)
         return;

      std::vector<char> buf(number_of_frames_to_write * stream.get_format().frame_size());
      stream.read(&buf[0], number_of_frames_to_write);
      size_t written = ad->write(&buf[0], number_of_frames_to_write);
      assert(written == number_of_frames_to_write);
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
   input_stream& stream;
   size_t current_sample;
   operation_cancelation oc;
};

snd_pcm_format_t wave_bps_to_alsa_format(boost::uint16_t sample_size)
{
   switch (sample_size)
   {
   case 1:
      return SND_PCM_FORMAT_U8;
   case 2:
      return SND_PCM_FORMAT_S16_LE;
   default:
      throw std::runtime_error("unknown bits per sample value");
   }
}

int main(int , char *[])
{
   try
   {
      input_stream_sp stream = open_wave_file("/home/ivan/d/alsa/asoundpp/1.wav");

      std::cout << "bits per sample: " << stream->get_format().sample_size * 8 << std::endl;
      std::cout << "channels:        " << stream->get_format().channels        << std::endl;
      std::cout << "sample rate:     " << stream->get_format().sample_rate     << std::endl;

      asound::global_config_cleanup cfg_cleanup;
      boost::asio::io_service io_service;

      asound::pcm::device d("default", SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);

      d.set_params(wave_bps_to_alsa_format(stream->get_format().sample_size),
                   SND_PCM_ACCESS_RW_INTERLEAVED,
                   stream->get_format().channels,
                   stream->get_format().sample_rate,
                   true,
                   500000);

      pcm_test pcm_test(io_service, d, *stream);

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
