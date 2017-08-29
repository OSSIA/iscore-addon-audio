#include "TChannelAudioStream.hpp"

TChannelAudioStream::TChannelAudioStream(TAudioStreamPtr as, const double *volume):
    TDecoratedAudioStream{as},
    fVolume{volume},
    fBuffer{TAudioGlobals::fBufferSize, as->Channels()}
{
}

TChannelAudioStream::~TChannelAudioStream()
{

}

long TChannelAudioStream::Read(FLOAT_BUFFER buffer, long framesNum, long framePos)
{
    float** in = (float**)alloca(fBuffer.GetChannels()*sizeof(float*));
    float** out = (float**)alloca(buffer->GetChannels()*sizeof(float*));
    fBuffer.GetFrame(0, in);
    buffer->GetFrame(framePos, out);

    UAudioTools::ZeroFloatBlk(in, fBuffer.GetSize(), fBuffer.GetChannels());

    long res = fStream->Read(&fBuffer, framesNum, 0);

    auto& vol = *fVolume;
    for(int chan = 0; chan < fBuffer.GetChannels(); chan++)
    {
        for(int j = 0; j < res; j++)
        {
            out[chan][j] += in[chan][j] * vol;
        }
    }

    return res;
}

long TChannelAudioStream::SetPos(long frames)
{
  return fStream->SetPos(frames);
}

TAudioStreamPtr TChannelAudioStream::Copy()
{
  return new TChannelAudioStream{fStream->Copy(), fVolume};
}
