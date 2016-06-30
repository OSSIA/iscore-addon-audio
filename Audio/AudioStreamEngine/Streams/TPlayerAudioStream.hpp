#pragma once
#include <TAudioStream.h>
#include <atomic>

class TGroupRenderer;
class TGroupAudioMixer;

// A stream that wraps a group renderer
class TPlayerAudioStream final : public TAudioStream
{
        TGroupRenderer& fRenderer;
        TGroupAudioMixer& fMixer;
        std::atomic_bool fScheduleReset{false};

    public:
        TPlayerAudioStream(
                TGroupRenderer&,
                TGroupAudioMixer&) ;

        ~TPlayerAudioStream() ;

        long Read(FLOAT_BUFFER buffer, long framesNum, long framePos) override;

        void Reset() override;

        // Cut the beginning of the stream
        TAudioStreamPtr CutBegin(long frames) override;

        // Length in frames
        long Length() override;

        long Channels() override;

        TAudioStreamPtr Copy() override;
};
