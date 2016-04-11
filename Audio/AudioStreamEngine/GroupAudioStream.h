#ifndef GROUPAUDIOSTREAM_H_2016_04_11
#define GROUPAUDIOSTREAM_H_2016_04_11
#include "LibAudioStreamMC++.h"

extern "C"
{
AudioRendererPtr MakeGroupPlayer();
AudioStream MakeGroupStream(AudioRendererPtr p);
void CloseAudioPlayer(AudioPlayerPtr ext_player); // In libaudiostreammc
}

#endif
