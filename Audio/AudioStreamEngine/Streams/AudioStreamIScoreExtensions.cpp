#include "TChannelAudioStream.hpp"
#include "TGroupRenderer.hpp"
#include "TGroupAudioMixer.hpp"
#include "TPlayerAudioStream.hpp"
#include "TFixedLoopAudioStream.hpp"
#include "TSendAudioStream.hpp"
#include "TReturnAudioStream.hpp"
#include "TGroupAudioStream.hpp"
#include "TSinusAudioStream.hpp"
#include "ExecutorAudioEffect.hpp"
#include <TAudioEffectInterface.h>
#include "TSimpleBufferAudioStream.hpp"
#include <TEffectAudioStream.h>

// TODO refactor this with the part in LibAudioStreamMC++.cpp
#ifdef _WIN32
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
typedef TAudioEffectInterfacePtr AudioEffect;

#if defined(__cplusplus) && !defined(_MSC_VER)
extern "C"
{
#endif
// APIs
AUDIOAPI AudioRendererPtr MakeGroupPlayer();
AUDIOAPI AudioStream MakeGroupStream(AudioRendererPtr p);
AUDIOAPI AudioStream MakeSinusStream(long length, float freq);

AUDIOAPI AudioStream MakeIScoreExecutor(AudioStream s, ossia::time_constraint& t);

AUDIOAPI AudioStream MakeSend(AudioStream s);
AUDIOAPI AudioStream MakeReturn(AudioStream s);
AUDIOAPI AudioStream MakeChannelSound(AudioStream s, double const * volume);
AUDIOAPI AudioStream MakeFixedLoopSound(AudioStream s, long maxlength);
AUDIOAPI AudioStream MakeSimpleBufferSound(float **buffer, long length, long channels);
AUDIOAPI SymbolicDate GenPriorisedSymbolicDate(AudioPlayerPtr /*player*/, int64_t prio);

#if defined(LILV_SHARED)
AUDIOAPI AudioEffect MakeLV2AudioEffect(LV2EffectContext*);
#endif
void CloseAudioPlayer(AudioPlayerPtr ext_player); // In libaudiostreammc

#if defined(__cplusplus) && !defined(_MSC_VER)
}
#endif
#include <QDebug>
// Implementations
AUDIOAPI AudioPlayerPtr MakeGroupPlayer()
{
    int res;
    const int n_out_channels = 2;

    TAudioGlobals::ClearLibError();

    IntAudioPlayerPtr player = static_cast<IntAudioPlayerPtr>(calloc(1, sizeof(AudioPlayer)));
    if (!player) {
        goto error;
    }

    player->fRenderer = new TGroupRenderer;
    res = player->fRenderer->Open(TAudioGlobals::fInput, n_out_channels, TAudioGlobals::fBufferSize, TAudioGlobals::fSampleRate);
    if (!player->fRenderer) {
        goto error;
    }

    player->fMixer = new TGroupAudioMixer{TAudioGlobals::fBufferSize, n_out_channels};
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

    return new TPlayerAudioStream{grp, mix};
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

AUDIOAPI AudioStream MakeIScoreExecutor(AudioStream s, ossia::time_constraint& t)
{
    return new TEffectAudioStream{s, new ExecutorAudioEffect{t}};
}

AUDIOAPI AudioStream MakeFixedLoopSound(
        AudioStream s,
        long maxlength)
{
    return new TFixedLoopAudioStream{s, maxlength};
}

AUDIOAPI AudioStream MakeSimpleBufferSound(float **buffer, long length, long channels)
{
    return new TSimpleBufferAudioStream{buffer, length, channels};
}

class TPriorisedSymbolicDate : public TSymbolicDate
{
    private:
        int64_t fPriority = 0;

    public :
        TPriorisedSymbolicDate(int64_t priority):
            TSymbolicDate(),
            fPriority{priority}
        {}
        TPriorisedSymbolicDate(audio_frame_t date, int64_t priority):
            TSymbolicDate(date),
            fPriority(priority)
        {}

        bool operator< (TSymbolicDate& date) override
        {
            auto other = dynamic_cast<TPriorisedSymbolicDate*>(&date);
            if(other)
                return fPriority < other->fPriority || TSymbolicDate::operator<(date);
            else
                return TSymbolicDate::operator<(date);
        }
};
AUDIOAPI SymbolicDate GenPriorisedSymbolicDate(AudioPlayerPtr /*player*/, int64_t prio)
{
    return new TPriorisedSymbolicDate(prio);
}

#if defined(LILV_SHARED)
AUDIOAPI AudioEffect MakeLV2AudioEffect(LV2EffectContext* c)
{
    try
    {
        if(c)
        {
            LV2Data dat{*c};
            if(dat.in_ports.size() == 2 && dat.out_ports.size() == 2)
            {
                return new StereoLV2AudioEffect{std::move(dat)};
            }
            else if(dat.in_ports.size() == 1 && dat.out_ports.size() == 1)
            {
                return new MonoLV2AudioEffect{std::move(dat)};
            }
        }
    }
    catch(const std::exception& e)
    {
        std::string s("Could not load LV2 plug-in: ");
        s += e.what();
        TAudioGlobals::AddLibError(s.c_str());
        return nullptr;
    }
    catch(...)
    {
    }

    TAudioGlobals::AddLibError("Could not load LV2 plug-in");
    return nullptr;
}
#endif
