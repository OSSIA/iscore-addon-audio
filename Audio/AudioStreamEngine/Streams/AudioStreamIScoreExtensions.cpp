#include "TChannelAudioStream.hpp"
#include "TGroupRenderer.hpp"
#include "TGroupAudioMixer.hpp"
#include "TPlayerAudioStream.hpp"
#include "TFixedLoopAudioStream.hpp"
#include "TSendAudioStream.hpp"
#include "TReturnAudioStream.hpp"
#include "TSinusAudioStream.hpp"
#include "ExecutorAudioEffect.hpp"
#include <TEffectAudioStream.h>

// TODO refactor this with the part in LibAudioStreamMC++.cpp
#ifdef WIN32
    #define	AUDIOAPI __declspec(dllexport)
#else
    #define	AUDIOAPI __attribute__ ((visibility("default")))
#endif

//// API
struct AudioPlayer {
    TAudioRendererPtr fRenderer;
    TExpAudioMixerPtr fMixer;
};
typedef void* AudioPlayerPtr;
typedef void* AudioRendererPtr;
typedef AudioPlayer* IntAudioPlayerPtr;
typedef TAudioStreamPtr AudioStream;

extern "C"
{
// APIs
AUDIOAPI AudioRendererPtr MakeGroupPlayer();
AUDIOAPI AudioStream MakeGroupStream(AudioRendererPtr p);
AUDIOAPI AudioStream MakeSinusStream(long length, float freq);

AUDIOAPI AudioStream MakeIScoreExecutor(AudioStream s, OSSIA::TimeConstraint& t);

AUDIOAPI AudioStream MakeSend(AudioStream s);
AUDIOAPI AudioStream MakeReturn(AudioStream s);
AUDIOAPI AudioStream MakeChannelSound(AudioStream s, double const * volume);
AUDIOAPI AudioStream MakeFixedLoopSound(AudioStream s, long maxlength);

void CloseAudioPlayer(AudioPlayerPtr ext_player); // In libaudiostreammc
}


// Implementations
AUDIOAPI AudioPlayerPtr MakeGroupPlayer()
{
    int res;

    TAudioGlobals::ClearLibError();

    IntAudioPlayerPtr player = static_cast<IntAudioPlayerPtr>(calloc(1, sizeof(AudioPlayer)));
    if (!player) {
        goto error;
    }

    player->fRenderer = new TGroupRenderer;
    res = player->fRenderer->Open(0, 2,
                                  TAudioGlobals::fBufferSize, TAudioGlobals::fSampleRate);
    if (!player->fRenderer) {
        goto error;
    }

    player->fMixer = new TGroupAudioMixer{TAudioGlobals::fBufferSize, 2};
    if (!player->fMixer) {
        goto error;
    }

    player->fRenderer->AddClient(player->fMixer);
    if (res == NO_ERR) {
        return player;
    }

error:
    CloseAudioPlayer(player);
    return 0;
}

AUDIOAPI AudioStream MakeGroupStream(AudioPlayerPtr p)
{
    auto player = static_cast<IntAudioPlayerPtr>(p);

    auto grp = dynamic_cast<TGroupRenderer*>(player->fRenderer);
    if(!grp)
        return nullptr;

    auto mix = dynamic_cast<TGroupAudioMixer*>(player->fMixer);
    if(!mix)
        return nullptr;

    return new TPlayerAudioStream{*grp, *mix};
}

AUDIOAPI AudioStream MakeSinusStream(long length, float freq)
{
    return new TSinusAudioStream{length, freq};
}

AUDIOAPI AudioStream MakeSend(AudioStream s)
{
    return new TSendAudioStream{static_cast<TAudioStreamPtr>(s)};
}

AUDIOAPI AudioStream MakeReturn(AudioStream send_stream)
{
    if(auto send = dynamic_cast<TSendAudioStream*>(send_stream.getPointer()))
        return new TReturnAudioStream{send};
    return nullptr;
}

AUDIOAPI AudioStream MakeChannelSound(AudioStream s, double const * volume)
{
    return new TChannelAudioStream{static_cast<TAudioStreamPtr>(s), volume};
}

AUDIOAPI AudioStream MakeIScoreExecutor(AudioStream s, OSSIA::TimeConstraint& t)
{
    return new TEffectAudioStream{s, new ExecutorAudioEffect{t}};
}

AUDIOAPI AudioStream MakeFixedLoopSound(
        AudioStream s,
        long maxlength)
{
    return new TFixedLoopAudioStream{s, maxlength};
}

