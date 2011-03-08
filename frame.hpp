#ifndef ASOUNDPP_FRAME_HPP
#define ASOUNDPP_FRAME_HPP

#include <boost/array.hpp>
#include "boost/integer/endian.hpp"

template <typename T>
struct sample_description_t;

template <>
struct sample_description_t<char>
{
   static const snd_pcm_format_t format = SND_PCM_FORMAT_S8;
};

template <>
struct sample_description_t<unsigned char>
{
   static const snd_pcm_format_t format = SND_PCM_FORMAT_U8;
};

template <>
struct sample_description_t<boost::integer::little16_t>
{
   static const snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
};

template <>
struct sample_description_t<boost::integer::ulittle16_t>
{
   static const snd_pcm_format_t format = SND_PCM_FORMAT_U16_LE;
};

template <>
struct sample_description_t<boost::integer::big16_t>
{
   static const snd_pcm_format_t format = SND_PCM_FORMAT_S16_BE;
};

template <>
struct sample_description_t<boost::integer::ubig16_t>
{
   static const snd_pcm_format_t format = SND_PCM_FORMAT_U16_BE;
};

template <typename T, unsigned Channels>
struct basic_frame_t
{
   static const snd_pcm_format_t format   = sample_description_t<T>::format;
   static const unsigned         channels = Channels;

   basic_frame_t()
   {
   }

   boost::array<T, Channels> ch;
};

template <typename T>
struct basic_frame_t<T, 1>
{
   static const snd_pcm_format_t format   = sample_description_t<T>::format;
   static const unsigned         channels = 1;

   basic_frame_t()
   {
   }

   explicit basic_frame_t(T ch0)
   {
      ch[0] = ch0;
   }
   
   boost::array<T, 1> ch;
};

template <typename T>
struct basic_frame_t<T, 2>
{
   static const snd_pcm_format_t format   = sample_description_t<T>::format;
   static const unsigned         channels = 2;

   basic_frame_t()
   {
   }

   basic_frame_t(T ch0, T ch1)
   {
      ch[0] = ch0;
      ch[1] = ch1;
   }

   boost::array<T, 2> ch;
};

typedef basic_frame_t<char         ,              1> frame1_s8;
typedef basic_frame_t<unsigned char,              1> frame1_u8;

typedef basic_frame_t<boost::integer::little16_t, 1> frame1_s16_le;
typedef basic_frame_t<boost::integer::little16_t, 1> frame1_u16_le;

typedef basic_frame_t<boost::integer::big16_t   , 1> frame1_s16_be;
typedef basic_frame_t<boost::integer::big16_t   , 1> frame1_u16_be;

typedef basic_frame_t<char         ,              2> frame2_s8;
typedef basic_frame_t<unsigned char,              2> frame2_u8;

typedef basic_frame_t<boost::integer::little16_t, 2> frame2_s16_le;
typedef basic_frame_t<boost::integer::little16_t, 2> frame2_u16_le;

typedef basic_frame_t<boost::integer::big16_t   , 2> frame2_s16_be;
typedef basic_frame_t<boost::integer::big16_t   , 2> frame2_u16_be;

#endif
