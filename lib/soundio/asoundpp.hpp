#ifndef SOUNDIO_ASOUNDPP_HPP
#define SOUNDIO_ASOUNDPP_HPP

#include <alsa/asoundlib.h>
#include <stdexcept>
#include <vector>

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
         device(char const* device_name, snd_pcm_stream_t stream, int mode = 0);
         ~device();

         snd_pcm_t* get() const;

         info get_info() const;

         void prepare();
         void drain();
         void set_hw_params(hw_params const& p);
         void set_params(snd_pcm_format_t format,        // e.g. SND_PCM_FORMAT_S16
                         snd_pcm_access_t access,        // e.g. SND_PCM_ACCESS_RW_INTERLEAVED
                         unsigned         channels,      // e.g. 2
                         unsigned         rate,          // e.g. 44100
                         bool             soft_resample,
                         unsigned         latency);      // (in us) e.g. 500000
         void writei(void const*, snd_pcm_uframes_t);
         snd_pcm_uframes_t readi(void*, snd_pcm_uframes_t);

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

#endif
