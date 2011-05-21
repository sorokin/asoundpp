#ifndef ASOUNDPP_ASOUNDPP_HPP
#define ASOUNDPP_ASOUNDPP_HPP

#include <alsa/asoundlib.h>
#include <stdexcept>
#include <sstream>
#include <vector>

#include "frame.hpp"

namespace asound
{
   namespace pcm
   {
      struct info;
      struct hw_params;
      struct device;

      struct underrun : std::runtime_error
      {
         underrun(std::string const&);
      };

      struct info
      {
         info();
         ~info();

         info(info const&);
         info& operator=(info const&);

         snd_pcm_info_t* get() const;

         unsigned         get_device() const;
         unsigned         get_subdevice() const;
         snd_pcm_stream_t get_stream() const;
         int              get_card() const;
         const char*      get_id() const;

      private:
         snd_pcm_info_t* p;
      };

      struct hw_params
      {
         hw_params();
         ~hw_params();

         hw_params(hw_params const&);
         hw_params& operator=(hw_params const&);

         snd_pcm_hw_params_t* get() const;

         void any(device const&);
         void set_access(device const&, snd_pcm_access_t);
         void set_format(device const&, snd_pcm_format_t);
         void set_rate(device const&, unsigned);
         void set_channels(device const&, unsigned);
         void set_periods(device const&, unsigned);
         void set_buffer_size(device const&, snd_pcm_uframes_t);

      private:
         snd_pcm_hw_params_t* p;
      };
      
      struct device
      {
         device(char const* name, snd_pcm_stream_t stream, int mode = 0);
         ~device();

         snd_pcm_t* get() const;

         info get_info() const;

         void prepare();
         void drain();
         void set_hw_params(hw_params const& p);
         void set_params(snd_pcm_format_t format,
                         snd_pcm_access_t access,
                         unsigned         channels,
                         unsigned         rate,
                         bool             soft_resample,
                         unsigned         latency);
         void writei(void const*, snd_pcm_uframes_t);

         std::vector<pollfd> poll_descriptors();
         unsigned short revents(pollfd*, size_t);
         unsigned short revents(std::vector<pollfd>&);

         snd_pcm_state_t state() const;

         size_t avail_update() const;

      private:
         device(device const&);
         device& operator=(device const&);

      private:
         snd_pcm_t* d;
      };
   }

   struct global_config_cleanup
   {
      global_config_cleanup();
      ~global_config_cleanup();

   private:
      global_config_cleanup(global_config_cleanup const&);
      global_config_cleanup& operator=(global_config_cleanup const&);
   };
}

asound::pcm::underrun::underrun(std::string const& msg)
   : runtime_error(msg)
{
}

asound::pcm::info::info()
{
   int r = snd_pcm_info_malloc(&p);
   if (r != 0)
      throw std::runtime_error("unable to allocate pcm info");
}

asound::pcm::info::~info()
{
   snd_pcm_info_free(p);
}

asound::pcm::info::info(info const& other)
{
   int r = snd_pcm_info_malloc(&p);
   if (r != 0)
      throw std::runtime_error("unable to allocate pcm info");

   snd_pcm_info_copy(p, other.p);
}

asound::pcm::info::info& asound::pcm::info::operator=(info const& rhs)
{
   snd_pcm_info_copy(p, rhs.p);
   return *this;
}

snd_pcm_info_t* asound::pcm::info::get() const
{
   return p;
}

unsigned asound::pcm::info::get_device() const
{
   return snd_pcm_info_get_device(p);
}

unsigned asound::pcm::info::get_subdevice() const
{
   return snd_pcm_info_get_subdevice(p);
}

snd_pcm_stream_t asound::pcm::info::get_stream() const
{
   return snd_pcm_info_get_stream(p);
}

int asound::pcm::info::get_card() const
{
   return snd_pcm_info_get_card(p);
}

const char* asound::pcm::info::get_id() const
{
   return snd_pcm_info_get_id(p);
}

asound::pcm::hw_params::hw_params()
{
   int r = snd_pcm_hw_params_malloc(&p);
   if (r != 0)
      throw std::runtime_error("unable to allocate hw_params");
}

asound::pcm::hw_params::~hw_params()
{
   snd_pcm_hw_params_free(p);
}

asound::pcm::hw_params::hw_params(hw_params const& other)
{
   int r = snd_pcm_hw_params_malloc(&p);
   if (r != 0)
      throw std::runtime_error("unable to allocate hw_params");

   snd_pcm_hw_params_copy(p, other.p);
}

asound::pcm::hw_params& asound::pcm::hw_params::operator=(hw_params const& rhs)
{
   // swap trick is not required here
   snd_pcm_hw_params_copy(p, rhs.p);
   return *this;
}

snd_pcm_hw_params_t* asound::pcm::hw_params::get() const
{
   return p;
}

void asound::pcm::hw_params::any(device const& d)
{
   int r = snd_pcm_hw_params_any(d.get(), p);
   if (r < 0)
   {
      std::stringstream ss;
      ss << "unable to fill hardware params for pcm device (error: " << r << ")";
      throw std::runtime_error(ss.str());
   }
}

void asound::pcm::hw_params::set_access(device const& d, snd_pcm_access_t a)
{
   int r = snd_pcm_hw_params_set_access(d.get(), p, a);
   if (r != 0)
      throw std::runtime_error("unable to set hardware params access type");
}

void asound::pcm::hw_params::set_format(device const& d, snd_pcm_format_t f)
{
   int r = snd_pcm_hw_params_set_format(d.get(), p, f);
   if (r != 0)
      throw std::runtime_error("unable to set hardware params format");
}

void asound::pcm::hw_params::set_rate(device const& d, unsigned rate)
{
   int r = snd_pcm_hw_params_set_rate(d.get(), p, rate, 0);
   if (r != 0)
   {
      std::stringstream ss;
      ss << "unable to set sample rate to " << rate;
      throw std::runtime_error(ss.str());
   }
}

void asound::pcm::hw_params::set_channels(device const& d, unsigned channels_num)
{
   int r = snd_pcm_hw_params_set_channels(d.get(), p, channels_num);
   if (r != 0)
   {
      std::stringstream ss;
      ss << "unable to set number of channels to " << channels_num;
      throw std::runtime_error(ss.str());
   }
}

void asound::pcm::hw_params::set_periods(device const& d, unsigned periods)
{
   int r = snd_pcm_hw_params_set_periods(d.get(), p, periods, 0);
   if (r != 0)
   {
      std::stringstream ss;
      ss << "unable to set number of periods to " << periods;
      throw std::runtime_error(ss.str());
   }
}

void asound::pcm::hw_params::set_buffer_size(device const& d, snd_pcm_uframes_t size)
{
   int r = snd_pcm_hw_params_set_buffer_size(d.get(), p, size);
   if (r != 0)
   {
      std::stringstream ss;
      ss << "unable to set buffer size to " << size << " frames";
      throw std::runtime_error(ss.str());
   }
}

asound::pcm::device::device(char const* name, snd_pcm_stream_t stream, int mode)
{
   int r = snd_pcm_open(&d, name, stream, mode);
   if (r != 0)
   {
      std::stringstream ss;
      ss << "unable to open pcm device \"" << name << "\" (error: " << r << ")";
      throw std::runtime_error(ss.str());
   }
}

asound::pcm::device::~device()
{
   snd_pcm_drop(d);
}

snd_pcm_t* asound::pcm::device::get() const
{
   return d;
}

asound::pcm::info asound::pcm::device::get_info() const
{
   asound::pcm::info ret;
   snd_pcm_info(d, ret.get());
   return ret;
}

void asound::pcm::device::prepare()
{
   int r = snd_pcm_prepare(d);
   if (r != 0)
      throw std::runtime_error("unable to prepare pcm device");
}

void asound::pcm::device::drain()
{
   int r = snd_pcm_drain(d);
   if (r != 0)
      throw std::runtime_error("unable to drain device");
}

void asound::pcm::device::set_hw_params(hw_params const& p)
{
   int r = snd_pcm_hw_params(d, p.get());
   if (r != 0)
      throw std::runtime_error("unable to set hardware params to pcm device");
}

void asound::pcm::device::set_params(snd_pcm_format_t format,
                                     snd_pcm_access_t access,
                                     unsigned         channels,
                                     unsigned         rate,
                                     bool             soft_resample,
                                     unsigned         latency)
{
   int r = snd_pcm_set_params(d, format, access, channels, rate, soft_resample ? 1 : 0, latency);
   if (r != 0)
      throw std::runtime_error("unable to set params to pcm device");
}

void asound::pcm::device::writei(void const* data, snd_pcm_uframes_t size)
{
   for (;;)
   {
      int r = snd_pcm_writei(d, data, size);
      if (r < 0)
      {
         std::stringstream ss;
         ss << "failed to write interleaved frame to pcm device (error: " << r << ")";
         switch (r)
         {
         case -EBADFD:
            ss << " pcm device is not in the right state";
            break;
         case -EPIPE:
            ss << " an underrun occurred";
            throw underrun(ss.str());
         case -ESTRPIPE:
            ss << " a suspend event occurred";
            break;
         }

         throw std::runtime_error(ss.str());
      }

      unsigned ur = (unsigned)r;
      if (ur == size)
         return;

      data = (char const*)data + ur;
      size -= ur;
   }
}

std::vector<pollfd> asound::pcm::device::poll_descriptors()
{
   std::vector<pollfd> r;

   int count = snd_pcm_poll_descriptors_count(d);
	if (count <= 0)
		throw std::runtime_error("negative or zero poll descriptors count!");

   r.resize((size_t)count);

   int err = snd_pcm_poll_descriptors(d, &r[0], (unsigned int)count);
	if (err < 0)
		throw std::runtime_error("can't get poll descriptors for pcm");

   return r;
}

unsigned short asound::pcm::device::revents(pollfd* fds, size_t size)
{
   unsigned short revent;
   int err = snd_pcm_poll_descriptors_revents(d, fds, size, &revent);
   if (err < 0)
      throw std::runtime_error("can't get revents");
   return revent;
}

unsigned short asound::pcm::device::revents(std::vector<pollfd>& fds)
{
   assert(fds.size() != 0);
   return revents(&fds[0], fds.size());
}

snd_pcm_state_t asound::pcm::device::state() const
{
   return snd_pcm_state(d);
}

size_t asound::pcm::device::avail_update() const
{
   snd_pcm_sframes_t r = snd_pcm_avail_update(d);
   if (r < 0)
      throw std::runtime_error("avail_update failed");
   return static_cast<size_t>(r);
}

asound::global_config_cleanup::global_config_cleanup()
{
}

asound::global_config_cleanup::~global_config_cleanup()
{
   snd_config_update_free_global();
}

#endif
