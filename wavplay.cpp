//#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include "asound_async.hpp"

#include <iostream>
#include <vector>

#include <boost/bind.hpp>
#include <boost/optional.hpp>

#include "input_stream.h"

struct set_raw_terminal
{
   set_raw_terminal(int fd)
      : fd(fd)
   {
      tcgetattr(fd, &oldt);
      termios newt = oldt;
      newt.c_lflag &= ~(ECHO | ICANON);
      tcsetattr(fd, TCSANOW, &newt);
   }

   ~set_raw_terminal()
   {
      tcsetattr(fd, TCSANOW, &oldt);
   }

private:
   int fd;
   termios oldt;
};

bool end_of_esc(char c)
{
   return (c >= '@' && c <= '~');
}

void printchar(std::ostream& s, char c)
{
   if (c <= ' ')
      s << "(" << (int)c << ")";
   else
      s << c;
}

struct keyboard_handler
{
   typedef boost::function<void (std::string const&)> on_key_t;

   keyboard_handler(boost::asio::io_service& io_service, int fd, on_key_t const& on_key)
      : input(io_service, ::dup(fd))
      , on_key(on_key)
   {
      start_read_input();
   }

   void handle_read_input(const boost::system::error_code& error, std::size_t length)
   {
      if (!error)
      {
         std::copy(input_buf.begin(), input_buf.begin() + length, std::back_inserter(output_buf));
         process_output_buf();
         start_read_input();
      }
      else
         std::cerr << "failed to read input (" << error << "), keyboard will be disabled" << std::endl;
   }

   void start_read_input()
   {
      typedef boost::function<void (boost::system::error_code, std::size_t length)> func_t;

      func_t f = boost::bind(&keyboard_handler::handle_read_input, this, _1, _2);
      input.async_read_some(boost::asio::buffer(input_buf), oc.wrap(f));
   }

   void process_output_buf()
   {
      for (;;)
      {
         size_t sz = output_buf.size();
         if (sz == 0)
            break;

         if (output_buf[0] != '\x1b')
         {
            callback_once(1);
            continue;
         }

         if (sz == 1)
            break;

         if (output_buf[1] == '[')
         {
            std::string::iterator i = std::find_if(output_buf.begin() + 2, output_buf.end(), &end_of_esc);
            if (i == output_buf.end())
               break;

            callback_once(i - output_buf.begin() + 1);
         }
         else if (end_of_esc(output_buf[1]))
         {
            callback_once(2);
            continue;
         }
         else
         {
            std::cerr << "unknown escape sequence: 27 " << (int)output_buf[1] << std::endl;
            output_buf.erase(output_buf.begin(), output_buf.begin() + 2); // do something to ignore this error
         }
      }
   }

   void callback_once(size_t n)
   {
      std::string t(output_buf.begin(), output_buf.begin() + n);
      output_buf.erase(output_buf.begin(), output_buf.begin() + n);
      // TODO: do something to allow destroying from callback
      on_key(t);
   }

private:
   boost::asio::posix::stream_descriptor input;
   on_key_t on_key;
   operation_cancelation oc;
   boost::array<char, 16> input_buf;
   std::string output_buf;
};

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
      , kh(boost::in_place(boost::ref(io_service), STDIN_FILENO, boost::bind(&pcm_test::on_keyboard, this, _1)))
      , stream(stream)
   {
      do_signal_t ds = boost::bind(&pcm_test::close_device, this);
      ss.async_wait(oc.wrap(ds));
   }

private:
   void on_keyboard(std::string const& seq)
   {
#if 0
      for (size_t i = 0; i != seq.size(); ++i)
         printchar(std::cerr, seq[i]);
      std::cerr << "\n";
#endif
      if (seq == "q")
         close_device();
      else if (seq == "\x1b[D")
      {
         size_t p = stream.get_position();
         size_t seek_size = stream.get_format().sample_rate * 5;
         if (p < seek_size)
            p = 0;
         else
            p -= seek_size;
         stream.seek(p);
      }
      else if (seq == "\x1b[C")
      {
         size_t p = stream.get_position();
         size_t s = stream.number_of_frames();
         size_t seek_size = stream.get_format().sample_rate * 5;
         if ((s - p) < seek_size)
            p = s;
         else
            p += seek_size;
         stream.seek(p);
      }
   }

   void do_write()
   {
      assert(ad);

      if (stream.number_of_frames() == stream.get_position())
      {
         // how to drain device asynchonously?
         close_device();
         return;
      }

      try
      {
         size_t number_of_frames_to_write = std::min(stream.number_of_frames() - stream.get_position(), ad->avail_update());
         if (number_of_frames_to_write == 0)
            return;

         std::vector<char> buf(number_of_frames_to_write * stream.get_format().frame_size());
         stream.read(&buf[0], number_of_frames_to_write);
         size_t written = ad->write(&buf[0], number_of_frames_to_write);
         assert(written == number_of_frames_to_write);
      }
      catch (std::exception const& e)
      {
         std::cerr << "error: " << e.what() << std::endl;
         close_device();
      }
   }

   void do_signal(const boost::system::error_code&, int /*sig_num*/)
   {
      ad = boost::none;
      kh = boost::none;
   }

   void close_device()
   {
      ss.cancel();
      ad = boost::none;
      kh = boost::none;
   }

private:
   boost::asio::signal_set ss;
   boost::optional<asound::pcm::async_device> ad;
   boost::optional<keyboard_handler> kh;
   input_stream& stream;
   size_t current_sample;
   operation_cancelation oc;
};

int main(int , char *[])
{
   try
   {
      //input_stream_sp stream = open_wave_file("/home/ivan/d/alsa/asoundpp/1.wav");
      input_stream_sp stream = open_flac_file("/home/ivan/d/alsa/1.flac");

      std::cerr << "format:          " << snd_pcm_format_name(stream->get_format().fmt)
                << " (" << snd_pcm_format_description(stream->get_format().fmt) << ")" <<std::endl;
      std::cerr << "channels:        " << stream->get_format().channels        << std::endl;
      std::cerr << "sample rate:     " << stream->get_format().sample_rate     << std::endl;

      asound::global_config_cleanup cfg_cleanup;
      boost::asio::io_service io_service;

      asound::pcm::device d("default", SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);

      d.set_params(stream->get_format().fmt,
                   SND_PCM_ACCESS_RW_INTERLEAVED,
                   stream->get_format().channels,
                   stream->get_format().sample_rate,
                   true,
                   500000);

      set_raw_terminal st(STDIN_FILENO);

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
