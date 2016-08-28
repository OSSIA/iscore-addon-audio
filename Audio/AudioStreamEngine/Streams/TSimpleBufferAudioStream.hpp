#pragma once
#include <TAudioStream.h>
#include "TBufferManager.hpp"

class TSimpleBufferAudioStream final :
        public TAudioStream
{
        float** fData{};
        int64_t fStreamLength{};
        int64_t fChannels{};

        int64_t fPos{};

    public:
        TSimpleBufferAudioStream(float** data, int64_t size, int64_t channels);

        ~TSimpleBufferAudioStream();

        long Read(FLOAT_BUFFER buffer, long framesNum, long framePos) override;

        long Length() override;
        long Channels() override;
        void Reset() override;

        TAudioStreamPtr CutBegin(long frames);

        TAudioStreamPtr Copy() override;
};

using TSimpleBufferAudioStreamPtr = LA_SMARTP<TSimpleBufferAudioStream>;
