#pragma once
#include <TAudioStream.h>

/** This loop has no iteration limit , but has a loop length limit
since the underlying stream may be infinite. */
class TFixedLoopAudioStream final : public TDecoratedAudioStream
{
    private:
        const long fMaxFrame;
        long fPos{};
        long fStreamLength{};

    public:
        TFixedLoopAudioStream(TAudioStreamPtr stream, long length);

        virtual ~TFixedLoopAudioStream();

        long Read(FLOAT_BUFFER buffer, long framesNum, long framePos) override;

        void Reset() override;

        TAudioStreamPtr CutBegin(long frames) override;

        long Length() override;

        TAudioStreamPtr Copy() override;

        long SetPos(long frames) override;
};
