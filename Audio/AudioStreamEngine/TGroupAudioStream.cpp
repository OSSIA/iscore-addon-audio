#include "TGroupAudioStream.hpp"

#include "TExpAudioMixer.h"
#include <3rdparty/libaudiostream/src/UAudioTools.h>
long TGroupRenderer::OpenImp(long inputDevice, long outputDevice, long inChan, long outChan, long bufferSize, long sampleRate)
{
    return 0;
}

TGroupRenderer::TGroupRenderer(): TAudioRenderer()
{
    fInputBuffer = new float*[TAudioGlobals::fInput];
    fOutputBuffer = new float*[TAudioGlobals::fOutput];

    for (int i = 0; i < TAudioGlobals::fInput; i++) {
        fInputBuffer[i] = new float[TAudioGlobals::fBufferSize];
    }
    for (int i = 0; i < TAudioGlobals::fInput; i++) {
        fOutputBuffer[i] = new float[TAudioGlobals::fBufferSize];
    }
}

TGroupRenderer::~TGroupRenderer()
{
    for (int i = 0; i < TAudioGlobals::fInput; i++) {
        delete [] fInputBuffer[i];
    }
    for (int i = 0; i < TAudioGlobals::fInput; i++) {
        delete [] fOutputBuffer[i];;
    }

    delete [] fInputBuffer;
    delete [] fOutputBuffer;
}

long TGroupRenderer::Open(long inChan, long outChan, long bufferSize, long sampleRate)
{
    int inDevice = 0;
    int outDevice = 0;
    return OpenImp(inDevice, outDevice, inChan, outChan, bufferSize, sampleRate);
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
    return 0;
}

long TGroupRenderer::Pause()
{
    return Stop();
}

long TGroupRenderer::Cont()
{
    return Start();
}

void TGroupRenderer::Process()
{
    // We don't use shared buffers here.
    auto size = TAudioGlobals::fBufferSize;
    // Clear output buffers
    UAudioTools::ZeroFloatBlk(fOutputBuffer, size, fOutput);

    // Client callback are supposed to *mix* their result in outputs
    auto iter = fClientList.begin();
    while (iter != fClientList.end())
    {
        TAudioClientPtr client = (*iter).fRTClient;
        if (client)
        {
            client->AudioCallback(fInputBuffer, fOutputBuffer, size);
            iter++;
        }
        else
        {
            iter = fClientList.erase(iter);
        }
    }
}

float** TGroupRenderer::GetOutputBuffer() const
{ return fOutputBuffer; }

void TGroupRenderer::GetInfo(RendererInfoPtr info)
{
    // TODO
}

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

    UAudioTools::MixFrameToFrameBlk1(buffer->GetFrame(framePos, temp1),
                                     fRenderer.GetOutputBuffer(),
                                     framesNum, TAudioGlobals::fInput);
    return framesNum;
}

void TPlayerAudioStream::Reset()
{
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
AudioRendererPtr MakeGroupPlayer();
AudioStream MakeGroupStream(AudioRendererPtr p);
void CloseAudioPlayer(AudioPlayerPtr ext_player); // In libaudiostreammc

AudioPlayerPtr MakeGroupPlayer()
{
    int res;

    TAudioGlobals::ClearLibError();

    IntAudioPlayerPtr player = static_cast<IntAudioPlayerPtr>(calloc(1, sizeof(AudioPlayer)));
    if (!player) {
        goto error;
    }

    player->fRenderer = new TGroupRenderer;
    if (!player->fRenderer) {
        goto error;
    }

    player->fMixer = new TExpAudioMixer;
    if (!player->fMixer) {
        goto error;
    }

    player->fRenderer->AddClient(player->fMixer);
    res = player->fRenderer->Open(2, 2, 512, 44100);

    if (res == NO_ERR) {
        return player;
    }

error:
    CloseAudioPlayer(player);
    return 0;
}

AudioStream MakeGroupStream(AudioPlayerPtr p)
{
    auto renderer = static_cast<IntAudioPlayerPtr>(p)->fRenderer;
    if(auto grp = dynamic_cast<TGroupRenderer*>(renderer))
        return new TPlayerAudioStream{*grp};
    return nullptr;
}
}
