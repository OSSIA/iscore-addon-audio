#pragma once
#include <iscore/command/SerializableCommand.hpp>
#include <Audio/Commands/AudioCommandFactory.hpp>
#include <Audio/MediaFileHandle.hpp>
#include <iscore/tools/ModelPath.hpp>
namespace Audio
{
namespace Effect {
class EffectModel;
class ProcessModel;
class EffectFactory;
}

namespace Commands
{
class InsertEffect final : public iscore::SerializableCommand
{
           ISCORE_COMMAND_DECL(Audio::CommandFactoryName(), InsertEffect, "Insert effect")
    public:
        InsertEffect(
                const Effect::ProcessModel& model,
                const UuidKey<Effect::EffectFactory>& effectKind,
                const QString& text,
                int effectPos);

        void undo() const override;
        void redo() const override;

    protected:
        void serializeImpl(DataStreamInput & s) const override;
        void deserializeImpl(DataStreamOutput & s) override;

    private:
        Path<Effect::ProcessModel> m_model;
        Id<Effect::EffectModel> m_id;
        UuidKey<Effect::EffectFactory> m_effectKind;
        QString m_effect;
        int m_pos{};

};
}
}