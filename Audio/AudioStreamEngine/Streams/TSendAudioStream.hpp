#pragma once
#include <TAudioStream.h>
#include "TBufferManager.hpp"
/**
 * @brief The TSendAudioStream class
 *
 * Calls Process on the watched stream and make an inner copy.
 */
class TReturnAudioStream;
class TSendAudioStream final :
        public TDecoratedAudioStream
{
        TBufferManager fBuffers;
        TSharedNonInterleavedAudioBuffer<float> fTempBuffer;
        int fCount = 0;
        std::vector<TReturnAudioStream*> fReturns;

    public:
        TSendAudioStream(TAudioStreamPtr as);

        ~TSendAudioStream();

        long Read(FLOAT_BUFFER buffer, long framesNum, long framePos) override;

        TSharedNonInterleavedAudioBuffer<float>& GetOutputBuffer();

        int GetReadBufferCount() const;

        void Reset() override;

        TAudioStreamPtr Copy() override;

        void RegisterReturn(TReturnAudioStream*);
        void UnregisterReturn(TReturnAudioStream*);
};
using TSendAudioStreamPtr = LA_SMARTP<TSendAudioStream>;
