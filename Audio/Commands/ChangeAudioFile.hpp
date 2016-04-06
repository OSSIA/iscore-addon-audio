#pragma once
#include <iscore/command/SerializableCommand.hpp>
#include <Audio/Commands/AudioCommandFactory.hpp>
#include <Audio/MediaFileHandle.hpp>
namespace Audio
{
namespace SoundProcess {
class ProcessModel;
}

namespace Commands
{
class ChangeAudioFile : public iscore::SerializableCommand
{
           ISCORE_COMMAND_DECL(Audio::CommandFactoryName(), ChangeAudioFile, "Change audio file")
    public:
        ChangeAudioFile(
                Path<SoundProcess::ProcessModel>&& model,
                const QString& text):
            m_model{std::move(model)},
            m_new{text}
        {
            m_old = m_model.find().file();
        }

        void undo() const override
        {
            m_model.find().setFile(m_old);
        }

        void redo() const override
        {
            m_model.find().setFile(m_new);
        }


    protected:
        void serializeImpl(DataStreamInput & s) const override
        {
            s << m_model << m_old << m_new;
        }
        void deserializeImpl(DataStreamOutput & s) override
        {
            s >> m_model >> m_old >> m_new;
        }

    private:
        Path<SoundProcess::ProcessModel> m_model;
        MediaFileHandle m_old, m_new;
};
}
}
