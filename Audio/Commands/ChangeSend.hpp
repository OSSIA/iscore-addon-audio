#pragma once
#include <score/command/Command.hpp>
#include <Audio/Commands/AudioCommandFactory.hpp>
#include <score/model/path/Path.hpp>
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
class ChangeSend final : public score::Command
{
           SCORE_COMMAND_DECL(Audio::CommandFactoryName(), ChangeSend, "Change send")
    public:
        ChangeSend(
               const Return::ProcessModel& model,
                const Path<Send::ProcessModel>& text);

        void undo(const score::DocumentContext& ctx) const override;
        void redo(const score::DocumentContext& ctx) const override;

    protected:
        void serializeImpl(DataStreamInput & s) const override;
        void deserializeImpl(DataStreamOutput & s) override;

    private:
        Path<Return::ProcessModel> m_model;
        Path<Send::ProcessModel> m_old, m_new;
};
}
}
