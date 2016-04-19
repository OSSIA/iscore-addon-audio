#ifndef GROUPAUDIOSTREAM_H_2016_04_11
#define GROUPAUDIOSTREAM_H_2016_04_11
#include "LibAudioStreamMC++.h"
extern "C"
{
AudioRendererPtr MakeGroupPlayer();
AudioStream MakeGroupStream(AudioRendererPtr p);

AudioStream MakeSinusStream(long length, float freq);

AudioStream MakeSend(AudioStream s);
AudioStream MakeReturn(AudioStream s);
}

#endif
