#ifndef GROUPAUDIOSTREAM_H_2016_04_11
#define GROUPAUDIOSTREAM_H_2016_04_11
#include "LibAudioStreamMC++.h"

extern "C"
{
AudioRendererPtr MakeGroupPlayer();
AudioStream MakeGroupStream(AudioRendererPtr p);
}

#endif
