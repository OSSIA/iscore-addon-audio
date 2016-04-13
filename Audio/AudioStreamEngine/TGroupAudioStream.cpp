#include "TGroupAudioStream.hpp"

#include "TSharedBuffers.h"
#include <iostream>
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
    if(fInputBuffer)
    {
        for (int i = 0; i < fInput; i++) {
            delete [] fInputBuffer[i];
        }
    }

    if(fOutputBuffer)
    {
        for (int i = 0; i < fOutput; i++) {
            delete [] fOutputBuffer[i];
        }
    }

    delete [] fInputBuffer;
    delete [] fOutputBuffer;
}

long TGroupRenderer::Open(long inChan, long outChan, long bufferSize, long sampleRate)
{
    TAudioRenderer::Open(inChan, outChan, bufferSize, sampleRate);

    fInputBuffer = new float*[inChan];
    fOutputBuffer = new float*[outChan];

    for (int i = 0; i < inChan; i++) {
        fInputBuffer[i] = new float[bufferSize];
    }
    for (int i = 0; i < outChan; i++) {
        fOutputBuffer[i] = new float[bufferSize];
    }

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

    public:
        TSharedBufferLocker(float** in, float** out)
        {
            // TODO maybe we should also save the number of channels ... ?
            TSharedBuffers::SetInBuffer(in);
            TSharedBuffers::SetOutBuffer(out);
        }

        ~TSharedBufferLocker()
        {
            TSharedBuffers::SetInBuffer(fPrevInBuffer);
            TSharedBuffers::SetOutBuffer(fPrevOutBuffer);
        }
};

void TGroupRenderer::Process()
{
    TSharedBufferLocker shared_buffers{fInputBuffer, fOutputBuffer};
    auto frames = fBufferSize;

    // Clear output buffers
    UAudioTools::ZeroFloatBlk(fOutputBuffer, frames, fOutput); // TODO set correct channel count
    // Client callback are supposed to *mix* their result in outputs
    auto iter = fClientList.cbegin();
    while (iter != fClientList.cend())
    {
        if (auto client = iter->fRTClient)
        {
            client->AudioCallback(fInputBuffer, fOutputBuffer, frames);
            ++iter;
        }
        else
        {
            iter = fClientList.erase(iter);
        }
    }
    fInfo.fCurFrame += frames;
    fInfo.fCurUsec = ConvertSample2Usec(fInfo.fCurFrame);
}

float** TGroupRenderer::GetOutputBuffer() const
{ return fOutputBuffer; }

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
    return TAudioGlobals::fInput;
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

    fRenderer.Process();
    auto out_buffer = fRenderer.GetOutputBuffer();

    UAudioTools::MixFrameToFrameBlk1(buffer->GetFrame(framePos, temp1),
                                     out_buffer,
                                     framesNum, TAudioGlobals::fInput);

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
    return TAudioGlobals::fInput;
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
    res = player->fRenderer->Open(TAudioGlobals::fInput, TAudioGlobals::fOutput,
                                  TAudioGlobals::fBufferSize, TAudioGlobals::fSampleRate);
    if (!player->fRenderer) {
        goto error;
    }

    player->fMixer = new TExpAudioMixer;
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
}
