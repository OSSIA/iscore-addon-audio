#ifndef GROUPAUDIOSTREAM_H_2016_04_11
#define GROUPAUDIOSTREAM_H_2016_04_11
#include "LibAudioStreamMC++.h"

namespace OSSIA { class TimeConstraint; }
extern "C"
{
AudioRendererPtr MakeGroupPlayer();
AudioStream MakeGroupStream(AudioRendererPtr p);

AudioStream MakeSinusStream(long length, float freq);

AudioStream MakeSend(AudioStream s);
AudioStream MakeReturn(AudioStream s);

AudioStream MakeChannelSound(AudioStream s, double const * volume);
AudioStream MakeIScoreExecutor(AudioStream s, OSSIA::TimeConstraint& t);

void LoopSound(AudioRendererPtr p, AudioStream s, SymbolicDate start, SymbolicDate stop);
}

#endif
