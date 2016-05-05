#include "TGroupAudioStream.hpp"

#include "TSharedBuffers.h"
#include <iostream>
#include <cmath>
#include <QDebug>
#include "TExpAudioMixer.h"
#include <3rdparty/libaudiostream/src/UAudioTools.h>
long TGroupRenderer::OpenImp(
        long inputDevice,
        long outputDevice,
        long inChan,
        long outChan,
        long bufferSize,
        long sampleRate)
{
    assert(false);
    // Shouldn't be used.
    return 0;
}

TGroupRenderer::TGroupRenderer(): TAudioRenderer()
{
}

TGroupRenderer::~TGroupRenderer()
{
}

long TGroupRenderer::Open(long inChan, long outChan, long bufferSize, long sampleRate)
{
    TAudioRenderer::Open(0, outChan, bufferSize, sampleRate);
    fBuffers = TBufferManager(0, outChan, bufferSize, sampleRate);
    return 0;
}

long TGroupRenderer::Close()
{
    return 0;
}

long TGroupRenderer::Start()
{
    return false;
}

long TGroupRenderer::Stop()
{
    for(const auto& clt : fClientList)
    {
        if(auto mixer = dynamic_cast<TExpAudioMixer*>(clt.fRTClient))
        {
            mixer->SetPos(0);
        }
    }
    fInfo.fCurFrame = 0;
    fInfo.fCurUsec = 0;
    return 0;
}

long TGroupRenderer::Pause()
{
    // TODO
    return Stop();
}

long TGroupRenderer::Cont()
{
    // TODO
    return Start();
}

/**
 * @brief The SharedBufferLocker class
 *
 * Will save and restore the state of shared buffers
 */
class TSharedBufferLocker
{
        float** fPrevInBuffer{TSharedBuffers::GetInBuffer()};
        float** fPrevOutBuffer{TSharedBuffers::GetOutBuffer()};
        TBufferedAudioStream* fSharedInput = TAudioGlobals::fSharedInput;

    public:
        TSharedBufferLocker(float** in, float** out)
        {
            // TODO maybe we should also save the number of channels ... ?
            TSharedBuffers::SetInBuffer(in);
            TSharedBuffers::SetOutBuffer(out);
            TAudioGlobals::fSharedInput = nullptr;
        }

        ~TSharedBufferLocker()
        {
            TSharedBuffers::SetInBuffer(fPrevInBuffer);
            TSharedBuffers::SetOutBuffer(fPrevOutBuffer);
            TAudioGlobals::fSharedInput = fSharedInput;
        }
};

void TGroupRenderer::Process(int64_t frames)
{
    TSharedBufferLocker shared_buffers{fBuffers.fInputBuffer, fBuffers.fOutputBuffer};

    // Clear output buffers
    UAudioTools::ZeroFloatBlk(fBuffers.fOutputBuffer, frames, fOutput); // TODO set correct channel count
    // Client callback are supposed to *mix* their result in outputs
    auto iter = fClientList.cbegin();
    while (iter != fClientList.cend())
    {
        if (auto client = iter->fRTClient)
        {
            client->AudioCallback(fBuffers.fInputBuffer, fBuffers.fOutputBuffer, frames);
            ++iter;
        }
        else
        {
            iter = fClientList.erase(iter);
        }
    }
    fInfo.fCurFrame += frames;
    auto cur_usec_double = ConvertSample2Usec(fInfo.fCurFrame);
    if(cur_usec_double <= (double) LONG_MAX)
        fInfo.fCurUsec = cur_usec_double;
    else
        fInfo.fCurUsec = LONG_MAX;
}

float** TGroupRenderer::GetOutputBuffer() const
{ return fBuffers.fOutputBuffer; }

void TGroupRenderer::GetInfo(RendererInfoPtr info)
{
    (*info) = fInfo;
}


//////////////////////////
//////////////////////////



TSinusAudioStream::TSinusAudioStream(long duration, float freq):
    fDuration{duration},
    fFreq{freq}
{

}

TSinusAudioStream::~TSinusAudioStream()
{

}

long TSinusAudioStream::Read(FLOAT_BUFFER buffer, long framesNum, long framePos)
{
    assert_stream(framesNum, framePos);

    float** temp1 = (float**)alloca(buffer->GetChannels()*sizeof(float*));
    auto out = buffer->GetFrame(framePos, temp1);

    auto cst = 2.*M_PI*fFreq / 44100.;
    for(int i = 0; i < framesNum; i++)
    {
        out[0][i] += std::sin( cst * fCurI ) * 0.25;
        out[1][i] += std::sin( cst * fCurI ) * 0.25;
        fCurI++;
    }

    return framesNum;
}

void TSinusAudioStream::Reset()
{
    fCurI = 0;
}

TAudioStreamPtr TSinusAudioStream::CutBegin(long frames)
{
    printf("TSinusAudioStream::CutBegin error\n");
    assert(false);
    return NULL;
}

long TSinusAudioStream::Length()
{
    return fDuration;
}

long TSinusAudioStream::Channels()
{
    return TAudioGlobals::fOutput;
}

TAudioStreamPtr TSinusAudioStream::Copy()
{
    printf("TSinusAudioStream::Copy error\n");
    assert(false);
    return NULL;
}




//////////////////////////
//////////////////////////

TPlayerAudioStream::TPlayerAudioStream(TGroupRenderer &renderer):
    fRenderer{renderer}
{

}

TPlayerAudioStream::~TPlayerAudioStream()
{

}

long TPlayerAudioStream::Read(FLOAT_BUFFER buffer, long framesNum, long framePos)
{
    assert_stream(framesNum, framePos);

    float** temp1 = (float**)alloca(buffer->GetChannels()*sizeof(float*));

    fRenderer.Process(framesNum);
    auto out_buffer = fRenderer.GetOutputBuffer();

    UAudioTools::MixFrameToFrameBlk1(buffer->GetFrame(framePos, temp1),
                                     out_buffer,
                                     framesNum, TAudioGlobals::fOutput);

    return framesNum;
}

void TPlayerAudioStream::Reset()
{
    fRenderer.Stop();
}

TAudioStreamPtr TPlayerAudioStream::CutBegin(long frames)
{
    printf("TPlayerAudioStream::CutBegin error\n");
    assert(false);
    return NULL;
}

long TPlayerAudioStream::Length()
{
    return LONG_MAX;
}

long TPlayerAudioStream::Channels()
{
    return TAudioGlobals::fOutput;
}

TAudioStreamPtr TPlayerAudioStream::Copy()
{
    printf("TPlayerAudioStream::Copy error\n");
    assert(false);
    return NULL;
}


#include <iscore_plugin_audio_export.h>
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
ISCORE_PLUGIN_AUDIO_EXPORT AudioRendererPtr MakeGroupPlayer();
ISCORE_PLUGIN_AUDIO_EXPORT AudioStream MakeGroupStream(AudioRendererPtr p);
ISCORE_PLUGIN_AUDIO_EXPORT AudioStream MakeSinusStream(long length, float freq);

ISCORE_PLUGIN_AUDIO_EXPORT AudioStream MakeSharedBus(AudioStream s);
ISCORE_PLUGIN_AUDIO_EXPORT AudioStream JoinSharedBus(AudioStream bus_stream);

ISCORE_PLUGIN_AUDIO_EXPORT AudioStream MakeSend(AudioStream s);
ISCORE_PLUGIN_AUDIO_EXPORT AudioStream MakeReturn(AudioStream s);
void CloseAudioPlayer(AudioPlayerPtr ext_player); // In libaudiostreammc

ISCORE_PLUGIN_AUDIO_EXPORT AudioPlayerPtr MakeGroupPlayer()
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

    // TODO make a wrapper that saves the stream commands
    // and sets them to an unknown symbolic date upon deletion.
    player->fMixer = new TExpAudioMixer{TAudioGlobals::fBufferSize, 2};
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

ISCORE_PLUGIN_AUDIO_EXPORT AudioStream MakeGroupStream(AudioPlayerPtr p)
{
    auto renderer = static_cast<IntAudioPlayerPtr>(p)->fRenderer;
    if(auto grp = dynamic_cast<TGroupRenderer*>(renderer))
        return new TPlayerAudioStream{*grp};
    return nullptr;
}

ISCORE_PLUGIN_AUDIO_EXPORT AudioStream MakeSinusStream(long length, float freq)
{
    return new TSinusAudioStream{length, freq};
}

ISCORE_PLUGIN_AUDIO_EXPORT AudioStream MakeSend(AudioStream s)
{
    return new TSendAudioStream{static_cast<TAudioStreamPtr>(s)};
}

ISCORE_PLUGIN_AUDIO_EXPORT AudioStream MakeReturn(AudioStream send_stream)
{
    if(auto send = dynamic_cast<TSendAudioStream*>(send_stream.getPointer()))
        return new TReturnAudioStream{send};
    return nullptr;
}

}
