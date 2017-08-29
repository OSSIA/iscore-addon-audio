#pragma once
#include <TAudioBuffer.h>
#include <TAudioStream.h>

/**
 * @brief The TChannelAudioStream class
 *
 * Mixer channel as an audiostream ; for now only volume
 */
class TChannelAudioStream final :
        public TDecoratedAudioStream
{
        double const * const fVolume{};
        TLocalNonInterleavedAudioBuffer<float> fBuffer;
    public:
        TChannelAudioStream(
                TAudioStreamPtr as,
                double const * volume);

        ~TChannelAudioStream();

        long Read(FLOAT_BUFFER buffer, long framesNum, long framePos) override;

        long SetPos(long frames) override;

        TAudioStreamPtr Copy() override;
};
using TChannelAudioStreamPtr = LA_SMARTP<TChannelAudioStream>;

