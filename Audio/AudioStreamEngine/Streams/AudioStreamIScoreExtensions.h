#ifndef AUDIOSTREAMISCOREEXTENSIONS_H_2016_04_11
#define AUDIOSTREAMISCOREEXTENSIONS_H_2016_04_11

#include "LibAudioStreamMC++.h"

#if defined(LILV_SHARED)
#include <lilv/lilv.h>
#endif
// This file provides a C-like API to allow use with the other
// functions in the C API of the LibAudioStream.
namespace ossia { class time_constraint; }

#if defined(__cplusplus) && !defined(_MSC_VER)
extern "C"
{
#endif
AudioRendererPtr MakeGroupPlayer();

// The stream takes ownership of the player
AudioStream MakeGroupStream(AudioRendererPtr p);

AudioStream MakeSinusStream(long length, float freq);

AudioStream MakeSend(AudioStream s);
AudioStream MakeReturn(AudioStream s);

AudioStream MakeChannelSound(AudioStream s, double const * volume);

AudioStream MakeSimpleBufferSound(float** buffer, long length, long channels);
AudioStream MakeIScoreExecutor(AudioStream s, ossia::time_constraint& t);

AudioStream MakeFixedLoopSound(AudioStream s, long maxlength);
SymbolicDate GenPriorisedSymbolicDate(AudioPlayerPtr /*player*/, int64_t prio);
#if defined(LILV_SHARED)
AudioEffect MakeLV2AudioEffect(const LilvPlugin*, LilvWorld*);
#endif

#if defined(__cplusplus) && !defined(_MSC_VER)
}
#endif

#endif
