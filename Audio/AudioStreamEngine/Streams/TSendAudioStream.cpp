#include "TSendAudioStream.hpp"

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
    fStream->Read(&fTempBuffer, framesNum, framePos);

    // Mix the local buffer into the output buffer
    float** temp1 = (float**)alloca(buffer->GetChannels()*sizeof(float*));
    UAudioTools::MixFrameToFrameBlk1(buffer->GetFrame(framePos, temp1),
                                     fBuffers.fOutputBuffer,
                                     framesNum,
                                     fBuffers.fOutput);

    fCount++;
    return framesNum;
}

float **TSendAudioStream::GetOutputBuffer() const
{
    return fBuffers.fOutputBuffer;
}

int TSendAudioStream::GetReadBufferCount() const
{
    return fCount;
}

void TSendAudioStream::Reset()
{
    fCount = 0;
    TDecoratedAudioStream::Reset();
}

TAudioStreamPtr TSendAudioStream::Copy()
{
    return new TSendAudioStream{fStream->Copy()};
}
