#pragma once
#include <TAudioStream.h>
#include "TBufferManager.hpp"
/**
 * @brief The TSendAudioStream class
 *
 * Calls Process on the watched stream and make an inner copy.
 */
class TSendAudioStream final :
        public TDecoratedAudioStream
{
        TBufferManager fBuffers;
        TSharedNonInterleavedAudioBuffer<float> fTempBuffer;
        int fCount = 0;

    public:
        TSendAudioStream(TAudioStreamPtr as);

        ~TSendAudioStream();

        long Read(FLOAT_BUFFER buffer, long framesNum, long framePos) override;

        float ** GetOutputBuffer() const;

        int GetReadBufferCount() const;

        void Reset() override;

        TAudioStreamPtr Copy() override;
};
using TSendAudioStreamPtr = LA_SMARTP<TSendAudioStream>;
