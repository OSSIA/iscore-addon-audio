#include "TSendAudioStream.hpp"
#include "TReturnAudioStream.hpp"
#include <signal.h>
TSendAudioStream::TSendAudioStream(TAudioStreamPtr as):
    TDecoratedAudioStream{as},
    fBuffers{0, as->Channels(), TAudioGlobals::fBufferSize, TAudioGlobals::fSampleRate},
    fTempBuffer{fBuffers.fOutputBuffer, fBuffers.fBufferSize, fBuffers.fOutput}
{
}

TSendAudioStream::~TSendAudioStream()
{

}

long TSendAudioStream::Read(FLOAT_BUFFER buffer, long framesNum, long framePos)
{
    // The process of the send should be transparent.
    // This way, the "send" can be re-used as a stream.
    // If there is nothing more to pull, the result is filled with silence.

    // Clear the local buffer
    UAudioTools::ZeroFloatBlk(fBuffers.fOutputBuffer, fBuffers.fBufferSize, fBuffers.fOutput);

    // Write the new data into the local buffer
    int res = fStream->Read(&fTempBuffer, framesNum, framePos);
    if(framePos != 0) std::cerr << "Send == " << framePos << "\n";

    // Mix the local buffer into the output buffer
    float** temp1 = (float**)alloca(buffer->GetChannels()*sizeof(float*));
    float** temp2 = (float**)alloca(fBuffers.fOutput *sizeof(float*));
    UAudioTools::MixFrameToFrameBlk1(buffer->GetFrame(framePos, temp1),
                                     fTempBuffer.GetFrame(framePos, temp2),
                                     framesNum,
                                     fBuffers.fOutput);

    fCount++;
    return res;
}

TSharedNonInterleavedAudioBuffer<float>& TSendAudioStream::GetOutputBuffer()
{
    return fTempBuffer;
}

int TSendAudioStream::GetReadBufferCount() const
{
    return fCount;
}

void TSendAudioStream::Reset()
{
    fCount = 0;
    for(TReturnAudioStream* ret : fReturns)
    {
        ret->ResetSendCount();
    }
    TDecoratedAudioStream::Reset();
}

TAudioStreamPtr TSendAudioStream::Copy()
{
    return new TSendAudioStream{fStream->Copy()};
}

long TSendAudioStream::SetPos(long frames)
{
  return fStream->SetPos(frames);
}

void TSendAudioStream::RegisterReturn(TReturnAudioStream* ret)
{
  auto it = std::find(fReturns.begin(), fReturns.end(), ret);
  if(it == fReturns.end())
    fReturns.push_back(ret);
}

void TSendAudioStream::UnregisterReturn(TReturnAudioStream* ret)
{
    auto it = std::find(fReturns.begin(), fReturns.end(), ret);
    if(it != fReturns.end())
        fReturns.erase(it);
}
