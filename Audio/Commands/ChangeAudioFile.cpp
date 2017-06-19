#include "ChangeAudioFile.hpp"

#include <iscore/model/path/PathSerialization.hpp>
#include <Audio/SoundProcess/SoundProcessModel.hpp>
namespace Audio
{
namespace Commands
{

ChangeAudioFile::ChangeAudioFile(
        const Sound::ProcessModel& model,
        const QString& text):
    m_model{model},
    m_new{text}
{
    m_old = model.file().name();
}

void ChangeAudioFile::undo(const iscore::DocumentContext& ctx) const
{
    m_model.find(ctx).setFile(m_old);
}

void ChangeAudioFile::redo(const iscore::DocumentContext& ctx) const
{
    m_model.find(ctx).setFile(m_new);
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
