#include "WavBlock.hpp"
namespace Audio
{
WavBlock::WavBlock(
        std::vector<float> audio,
        AudioEngine &params):
    AudioBlock{params},
    m_audio{std::move(audio)}
{

}

std::vector<float> WavBlock::data(int size, int buffer, int offset) const
{
    std::size_t min_n = buffer * size;
    std::size_t max_n = (1+buffer) * size;
    if(m_audio.size() >= max_n)
    {
        return std::vector<float>(m_audio.begin() + min_n, m_audio.begin() + max_n);
    }

    return {};
}
}
