#ifndef ASOUNDPP_SINE_WAVE_GENERATOR_HPP
#define ASOUNDPP_SINE_WAVE_GENERATOR_HPP

#include <math.h>

struct sine_wave_generator
{
   sine_wave_generator(short amplitude, double frequency, unsigned sample_rate)
      : amplitude(amplitude)
      , frequency(frequency)
      , sample_rate(sample_rate)
      , sample_num(0)
   {
   }

   short operator()()
   {
      return amplitude * sin(frequency * (double)sample_num++ / (double)sample_rate * 2 * M_PI);
   }

private:
   short amplitude;
   double frequency;
   unsigned sample_rate;
   unsigned sample_num;
};

#endif

