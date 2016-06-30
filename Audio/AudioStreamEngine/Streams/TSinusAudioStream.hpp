#pragma once
#include <TAudioStream.h>

class TSinusAudioStream final : public TAudioStream
{
        long fDuration{};
        float fFreq{};
        int fCurI = 0;

    public:
        TSinusAudioStream(long duration, float freq) ;

        ~TSinusAudioStream() ;

        long Read(FLOAT_BUFFER buffer, long framesNum, long framePos) override;

        void Reset() override;

        // Cut the beginning of the stream
        TAudioStreamPtr CutBegin(long frames) override;

        // Length in frames
        long Length() override;

        long Channels() override;

        TAudioStreamPtr Copy() override;
};


