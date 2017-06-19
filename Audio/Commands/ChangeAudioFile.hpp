#pragma once
#include <iscore/command/Command.hpp>
#include <Audio/Commands/AudioCommandFactory.hpp>
#include <Audio/MediaFileHandle.hpp>
#include <iscore/model/path/Path.hpp>
namespace Audio
{
namespace Sound {
class ProcessModel;
}

namespace Commands
{
class ChangeAudioFile final : public iscore::Command
{
           ISCORE_COMMAND_DECL(Audio::CommandFactoryName(), ChangeAudioFile, "Change audio file")
    public:
        ChangeAudioFile(
               const Sound::ProcessModel&,
               const QString& text);

        void undo(const iscore::DocumentContext& ctx) const override;
        void redo(const iscore::DocumentContext& ctx) const override;

    protected:
        void serializeImpl(DataStreamInput & s) const override;
        void deserializeImpl(DataStreamOutput & s) override;

    private:
        Path<Sound::ProcessModel> m_model;
        QString m_old, m_new;
};
}
}
