#pragma once
#include <TAudioStream.h>
#include "TSendAudioStream.hpp"

/**
 * @brief The TReturnAudioStream class
 *
 * Make a copy of the watched stream, and apply a copy on it
 */
class TReturnAudioStream final :
        public TAudioStream
{
        TSendAudioStreamPtr fSend;
        int fLastSendCount = -1;
        bool fStarted = false;
    public:
        TReturnAudioStream(TSendAudioStreamPtr as);

        ~TReturnAudioStream();

        long Read(FLOAT_BUFFER buffer, long framesNum, long framePos) override;

        void Reset() override;

        // Length in frames
        long Length() override;

        long Channels() override;

        TAudioStreamPtr Copy() override;

        void ResetSendCount();
};
