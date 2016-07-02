#include "TSinusAudioStream.hpp"
#if defined(_MSC_VER)
#define _USE_MATH_DEFINES
#include <math.h>
#else
#if !defined(M_PI)
#define M_PI (3.14159265358979323846)
#endif
#endif

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
