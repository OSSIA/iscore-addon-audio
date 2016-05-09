#pragma once
#include <iscore/command/SerializableCommand.hpp>
#include <Audio/Commands/AudioCommandFactory.hpp>
#include <iscore/tools/ModelPath.hpp>

namespace Audio
{
namespace Effect {
class FaustEffectModel;
}

namespace Commands
{
class EditFaustEffect final : public iscore::SerializableCommand
{
           ISCORE_COMMAND_DECL(Audio::CommandFactoryName(), EditFaustEffect, "Edit Faust effect")
    public:
        EditFaustEffect(
                const Effect::FaustEffectModel& model,
                const QString& text);

        void undo() const override;
        void redo() const override;

    protected:
        void serializeImpl(DataStreamInput & s) const override;
        void deserializeImpl(DataStreamOutput & s) override;

    private:
        Path<Effect::FaustEffectModel> m_model;
        QString m_old, m_new;

};
}
}
