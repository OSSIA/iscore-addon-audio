#include "TPlayerAudioStream.hpp"
#include "TGroupRenderer.hpp"
#include "TGroupAudioMixer.hpp"

TPlayerAudioStream::TPlayerAudioStream(
        TGroupRenderer& renderer,
        TGroupAudioMixer& mixer):
    fRenderer{renderer},
    fMixer{mixer}
{


}

TPlayerAudioStream::~TPlayerAudioStream()
{

}

long TPlayerAudioStream::Read(FLOAT_BUFFER buffer, long framesNum, long framePos)
{
    assert_stream(framesNum, framePos);
    if(fScheduleReset)
    {
        fRenderer.Stop();
        fMixer.Reset();
        fScheduleReset = false;
    }

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
    fScheduleReset = true;
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
