#pragma once
#include <Audio/AudioBlock.hpp>
#include <vector>

namespace Audio
{
class WavBlock : public AudioBlock
{
    public:
        std::vector<float> m_audio;

        WavBlock(
                std::vector<float> audio,
                AudioEngine& params);

        std::vector<float> data(int size, int buffer, int offset) const override;
};
}
