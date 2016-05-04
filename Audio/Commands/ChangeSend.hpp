#pragma once
#include <iscore/command/SerializableCommand.hpp>
#include <Audio/Commands/AudioCommandFactory.hpp>
#include <iscore/tools/ModelPath.hpp>
namespace Audio
{
namespace Send{
class ProcessModel;
}
namespace Return {
class ProcessModel;
}

namespace Commands
{
class ChangeSend final : public iscore::SerializableCommand
{
           ISCORE_COMMAND_DECL(Audio::CommandFactoryName(), ChangeSend, "Change send")
    public:
        ChangeSend(
                Path<Return::ProcessModel>&& model,
                const Path<Send::ProcessModel>& text);

        void undo() const override;
        void redo() const override;

    protected:
        void serializeImpl(DataStreamInput & s) const override;
        void deserializeImpl(DataStreamOutput & s) override;

    private:
        Path<Return::ProcessModel> m_model;
        Path<Send::ProcessModel> m_old, m_new;
};
}
}