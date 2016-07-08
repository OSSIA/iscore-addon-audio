#include "TFixedLoopAudioStream.hpp"

TFixedLoopAudioStream::TFixedLoopAudioStream(TAudioStreamPtr stream, long length):
    TDecoratedAudioStream(stream),
    fMaxFrame{length},
    fStreamLength{stream->Length()}
{
}

TFixedLoopAudioStream::~TFixedLoopAudioStream()
{

}

long TFixedLoopAudioStream::Length()
{
    return LONG_MAX;
}

TAudioStreamPtr TFixedLoopAudioStream::Copy()
{
    return new TFixedLoopAudioStream(fStream->Copy(), fMaxFrame);
}

long TFixedLoopAudioStream::SetPos(long frames)
{
    long len = fStream->Length();
    return fStream->SetPos(frames % len);
}

long TFixedLoopAudioStream::Read(FLOAT_BUFFER buffer, long framesNum, long framePos)
{
    assert_stream(framesNum, framePos);

    auto nextPos = fPos + framesNum;
    if(nextPos < fMaxFrame)
    {
        if(fPos >= fStreamLength)
        {
            // Only silence
        }
        else if(nextPos >= fStreamLength)
        {
            // Part read, part silence
            fStream->Read(buffer, nextPos - fStreamLength, framePos);
        }
        else
        {
            // Full read. TODO this should be the first if branch
            // since it ought to be the most common.
            fStream->Read(buffer, framesNum, framePos);
        }
        fPos = nextPos;
    }
    else
    {
        auto toRead = nextPos - fMaxFrame;
        // Read what we can
        fStream->Read(buffer, toRead, framePos);

        // Reset
        fStream->Reset();

        // Read the remainder
        auto remainder = framesNum - toRead;
        fPos = remainder;

        if(remainder > 0)
            fStream->Read(buffer, remainder, framePos + toRead);
    }

    return framesNum;
}

TAudioStreamPtr TFixedLoopAudioStream::CutBegin(long frames)
{
    // TODO
    assert(false);
    return {};
}

void TFixedLoopAudioStream::Reset()
{
    assert(fStream);
    fPos = 0;
    fStream->Reset();
}
