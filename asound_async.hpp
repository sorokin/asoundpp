#ifndef ASOUNDPP_ASOUND_ASYNC_HPP
#define ASOUNDPP_ASOUND_ASYNC_HPP

#include <vector>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <boost/utility/in_place_factory.hpp>

#include "asio.hpp"

#include "asoundpp.hpp"

namespace asound
{
   namespace pcm
   {
      int dup_syscall(int fd)
      {
         int r = ::dup(fd);
         if (r == -1)
            throw asio::system_error(asio::error_code(errno, asio::system_category()));
         return r;
      }

      struct async_device_impl
      {
         async_device_impl(asio::io_service& io_service,
                           asound::pcm::device& d)
            : io_service(io_service)
            , d(d)
            , sd(io_service)
         {
            std::vector<pollfd> ufds = d.poll_descriptors();
            if (ufds.size() != 1)
               throw std::logic_error("only one descriptor is supported");

            if (ufds[0].events != SUPPORTED_EVENT_TYPE)
               throw std::logic_error("unsupported event type");

            sd.assign(dup_syscall(ufds[0].fd));
         }

         asio::io_service& io_service;
         asound::pcm::device& d;

         asio::posix::stream_descriptor sd;

         static const int SUPPORTED_EVENT_TYPE = POLLNVAL | POLLERR | POLLIN;
      };

      struct async_device
      {
         typedef boost::function<void ()> on_write_t;
         typedef boost::function<void ()> on_error_t;

         async_device(asio::io_service& io_service,
                      asound::pcm::device& d,
                      on_write_t on_write,
                      on_error_t on_error)
            : impl(boost::in_place(boost::ref(io_service), boost::ref(d)))
            , w(io_service)
            , on_write(on_write)
            , on_error(on_error)
         {
            start_wait();
         }

         size_t write(void const* data, size_t size)
         {
            int err = snd_pcm_writei(impl->d.get(), data, size);
            if (err == -EPIPE)
            {
               impl->d.prepare();
               return 0;
            }
            else if (err == -EAGAIN)
            {
               return 0;
            }
            else if (err < 0)
            {
               impl->io_service.post(on_error);
               impl = boost::none;
               return 0;
            }
            else
            {
               return static_cast<size_t>(err);
            }
         }

      private:
         void start_wait()
         {
            impl->sd.async_read_some(asio::null_buffers(), boost::bind(&async_device::after_wait, this, _1, _2));
         }

         void after_wait(const asio::error_code& error,
                         std::size_t /*bytes_transferred*/)
         {
            if (error == asio::error::operation_aborted)
               return;

            if (!error)
            {
               pollfd ufds;
               ufds.fd = impl->sd.native_handle();
               ufds.events = async_device_impl::SUPPORTED_EVENT_TYPE;
               ufds.revents = POLLIN;

               short r = impl->d.revents(&ufds, 1);

               if (r & POLLERR)
               {
                  impl = boost::none;
                  on_error();
               }
               else if (r & POLLOUT)
               {
                  on_write();
                  start_wait();
               }
               else
                  start_wait();
            }
            else
            {
               impl = boost::none;
               on_error();
            }
         }

      private:
         boost::optional<async_device_impl> impl;
         asio::io_service::work w;
         on_write_t on_write;
         on_error_t on_error;
      };
   }
}

#endif // ASOUND_ASYNC_HPP
