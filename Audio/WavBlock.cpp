#include "WavBlock.hpp"
namespace Audio
{
WavBlock::WavBlock(
        AudioArray audio,
        AudioEngine &params):
    AudioBlock{params},
    m_audio{std::move(audio)}
{

}

AudioArray make_array(int channels, int buffer)
{
    AudioArray out;
    out.resize(channels);
    for(auto& vec : out)
        vec.resize(buffer);
    return out;
}

AudioArray WavBlock::data(int size, int buffer, int offset) const
{
    auto n_chan = m_audio.size();
    auto audio = make_array(n_chan, size);

    std::size_t min_n = buffer * size;
    std::size_t max_n = (1+buffer) * size;

    qDebug() << n_chan;
    for(std::size_t i_chan = 0; i_chan < n_chan; i_chan++)
    {
        auto& src_chan = m_audio[i_chan];
        auto& dest_chan = audio[i_chan];
        if(src_chan.size() >= max_n)
        {
            std::copy_n(src_chan.begin() + min_n, size, dest_chan.begin());
        }
    }

    return audio;
}
}
