#ifndef AUDIOSTREAMISCOREEXTENSIONS_H_2016_04_11
#define AUDIOSTREAMISCOREEXTENSIONS_H_2016_04_11

#include "LibAudioStreamMC++.h"

// This file provides a C-like API to allow use with the other
// functions in the C API of the LibAudioStream.
namespace ossia { class time_constraint; }
#if defined(__cplusplus) && !defined(_MSC_VER)
extern "C"
{
#endif
AudioRendererPtr MakeGroupPlayer();
AudioStream MakeGroupStream(AudioRendererPtr p);

AudioStream MakeSinusStream(long length, float freq);

AudioStream MakeSend(AudioStream s);
AudioStream MakeReturn(AudioStream s);

AudioStream MakeChannelSound(AudioStream s, double const * volume);
AudioStream MakeIScoreExecutor(AudioStream s, ossia::time_constraint& t);

AudioStream MakeFixedLoopSound(AudioStream s, long maxlength);
#if defined(__cplusplus) && !defined(_MSC_VER)
}
#endif

#endif
