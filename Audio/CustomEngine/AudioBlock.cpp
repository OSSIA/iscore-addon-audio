#include "AudioBlock.hpp"
#include "AudioEngine.hpp"
#include <array>
const Parameters<float>&AudioBlock::parameters() const
{ return m_engine.params; }

AudioBlock::~AudioBlock()
{
	if(!m_deleting)
		m_engine.removeHandle(this);
}

void AudioBlock::start()
{
	if(!m_deleting)
		m_engine.addHandle(this);
}

void AudioBlock::stop()
{
	if(!m_deleting)
		m_engine.removeHandle(this);
}
