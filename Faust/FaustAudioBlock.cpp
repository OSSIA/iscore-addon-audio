#include "FaustAudioBlock.hpp"
namespace Audio
{
namespace Faust
{

Block::Block(
        const QString& script,
        AudioEngine& params):
    AudioBlock{params}
{
    std::string target;
    m_faustFactory = createDSPFactoryFromString(
                         "name",
                         "phasor(f)   = f/48000 : (+,1.0:fmod) ~ _ ; "
                         "process = phasor(220) * 6.28 : sin;",//script.toStdString(),
                         0, nullptr, "", target);
    if(m_faustFactory)
    {
        m_faustPlug = createDSPInstance(m_faustFactory);
    }

    for(auto& vec : in_vec_cnt)
        vec.resize(parameters().bufferSize);
    for(auto& vec : out_vec_cnt)
        vec.resize(parameters().bufferSize);

}

Block::~Block()
{
    deleteDSPFactory(m_faustFactory);
}

static std::array<float*, 2> wrap_vector(const std::vector<std::vector<float>>& vec)
{
    ISCORE_ASSERT(vec.size() == 2);
    return {vec[0].data(), vec[1].data()};
}

AudioArray Block::data(int size, int buffer, int off) const
{
    if(m_faustPlug)
    {
        auto in = wrap_vector(in_vec_cnt);
        auto out = wrap_vector(out_vec_cnt);
        m_faustPlug->compute(
                    parameters().bufferSize, in.data(), out.data());

        return out_vec_cnt;
    }
    else
    {
        return {};
    }
}

}
}
