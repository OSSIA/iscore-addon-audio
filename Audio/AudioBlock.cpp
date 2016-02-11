#include "AudioBlock.hpp"
#include "AudioEngine.hpp"
#include <array>
const Parameters<float>&AudioBlock::parameters() const
{ return m_engine.params; }

AudioBlock::~AudioBlock()
{
    m_engine.removeHandle(this);

}

void AudioBlock::start()
{
    m_engine.addHandle(this);
}

void AudioBlock::stop()
{
    m_engine.removeHandle(this);
}
