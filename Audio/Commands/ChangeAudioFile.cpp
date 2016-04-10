#include "ChangeAudioFile.hpp"

#include <iscore/tools/ModelPathSerialization.hpp>
#include <Audio/SoundProcess/SoundProcessModel.hpp>
namespace Audio
{
namespace Commands
{

ChangeAudioFile::ChangeAudioFile(
        Path<Sound::ProcessModel>&& model,
        const QString& text):
    m_model{std::move(model)},
    m_new{text}
{
    m_old = m_model.find().file();
}

void ChangeAudioFile::undo() const
{
    m_model.find().setFile(m_old);
}

void ChangeAudioFile::redo() const
{
    m_model.find().setFile(m_new);
}

void ChangeAudioFile::serializeImpl(DataStreamInput& s) const
{
    s << m_model << m_old << m_new;
}

void ChangeAudioFile::deserializeImpl(DataStreamOutput& s)
{
    s >> m_model >> m_old >> m_new;
}

}
}
