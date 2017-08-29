#include "TPlayerAudioStream.hpp"
#include "TGroupRenderer.hpp"
#include "TGroupAudioMixer.hpp"

TPlayerAudioStream::TPlayerAudioStream(
        TGroupRenderer* renderer,
        TGroupAudioMixer* mixer):
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

    TGroupRenderer& renderer = *fRenderer;
    auto& mixer = *fMixer;
    if(fScheduleReset)
    {
        renderer.Stop();
        mixer.Reset();
        fScheduleReset = false;
    }

    float** temp1 = (float**)alloca(buffer->GetChannels()*sizeof(float*));

    renderer.Process(framesNum);
    auto out_buffer = renderer.GetOutputBuffer();

    RendererInfo info;
    renderer.GetInfo(&info);

    UAudioTools::MixFrameToFrameBlk1(buffer->GetFrame(framePos, temp1),
                                     out_buffer,
                                     framesNum, std::min(buffer->GetChannels(), info.fOutput));

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

long TPlayerAudioStream::SetPos(long frames)
{
  fMixer->SetPos(frames);
  return 0;
}
