#include "TSimpleBufferAudioStream.hpp"

TSimpleBufferAudioStream::TSimpleBufferAudioStream(float** data, int64_t size, int64_t channels):
    TAudioStream{},
    fData{data},
    fStreamLength{size},
    fChannels{channels}
{
}

TSimpleBufferAudioStream::~TSimpleBufferAudioStream()
{

}

long TSimpleBufferAudioStream::Length()
{
    return fStreamLength;
}

long TSimpleBufferAudioStream::Channels()
{
    return fChannels;
}

TAudioStreamPtr TSimpleBufferAudioStream::Copy()
{
    return new TSimpleBufferAudioStream{fData, fStreamLength, fChannels};
}

long TSimpleBufferAudioStream::Read(FLOAT_BUFFER buffer, long framesNum, long framePos)
{
    assert_stream(framesNum, framePos);

    float** src = (float**)alloca(buffer->GetChannels()*sizeof(float*));
    for (int i = 0; i < fChannels; i++) {
        src[i] = &fData[i][fPos];
    }
    float** temp_dst = (float**)alloca(buffer->GetChannels()*sizeof(float*));
    temp_dst = buffer->GetFrame(framePos, temp_dst);

    auto nextPos = fPos + framesNum;
    if(fPos >= fStreamLength)
    {
        // Only silence
    }
    else if(nextPos >= fStreamLength)
    {
        // Part read, part silence
        for(int64_t i = 0; i < fChannels; i++)
        {
            std::copy_n(src[i], framesNum - (nextPos - fStreamLength), temp_dst[i]);
        }
    }
    else
    {
        // Full read.
        for(int64_t i = 0; i < fChannels; i++)
        {
            std::copy_n(src[i], framesNum, temp_dst[i]);
        }
    }
    fPos = nextPos;

    return framesNum;
}

TAudioStreamPtr TSimpleBufferAudioStream::CutBegin(long frames)
{
    // TODO
    assert(false);
    return {};
}

void TSimpleBufferAudioStream::Reset()
{
    fPos = 0;
}
