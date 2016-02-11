#pragma once
#include <Audio/AudioBlock.hpp>
#include <vector>

namespace Audio
{
class WavBlock : public AudioBlock
{
    public:
        AudioArray m_audio;

        WavBlock(
                AudioArray audio,
                AudioEngine& params);

        AudioArray data(int size, int buffer, int offset) const override;
};
}
