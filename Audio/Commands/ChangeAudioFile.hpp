#pragma once
#include <iscore/command/SerializableCommand.hpp>
#include <Audio/Commands/AudioCommandFactory.hpp>
#include <Audio/MediaFileHandle.hpp>
#include <iscore/tools/ModelPath.hpp>
namespace Audio
{
namespace Sound {
class ProcessModel;
}

namespace Commands
{
class ChangeAudioFile final : public iscore::SerializableCommand
{
           ISCORE_COMMAND_DECL(Audio::CommandFactoryName(), ChangeAudioFile, "Change audio file")
    public:
        ChangeAudioFile(
                Path<Sound::ProcessModel>&& model,
                const QString& text);
       ChangeAudioFile(
                Path<Sound::ProcessModel>&& model,
                MediaFileHandle&& newfile);

        void undo() const override;

        void redo() const override;


    protected:
        void serializeImpl(DataStreamInput & s) const override;
        void deserializeImpl(DataStreamOutput & s) override;

    private:
        Path<Sound::ProcessModel> m_model;
        MediaFileHandle m_old, m_new;
};
}
}
