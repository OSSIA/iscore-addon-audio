#pragma once
#include <iscore/command/Command.hpp>
#include <Audio/Commands/AudioCommandFactory.hpp>
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
class InsertEffect final : public iscore::Command
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


// MOVEME
class RemoveEffect final : public iscore::Command
{
           ISCORE_COMMAND_DECL(Audio::CommandFactoryName(), RemoveEffect, "Remove effect")
    public:
        RemoveEffect(
                const Effect::ProcessModel& model,
                const Effect::EffectModel& effect);

        void undo() const override;
        void redo() const override;

    protected:
        void serializeImpl(DataStreamInput & s) const override;
        void deserializeImpl(DataStreamOutput & s) override;

    private:
        Path<Effect::ProcessModel> m_model;
        Id<Effect::EffectModel> m_id;
        QByteArray m_savedEffect;
        int m_pos{};
};


class MoveEffect final : public iscore::Command
{
           ISCORE_COMMAND_DECL(Audio::CommandFactoryName(), MoveEffect, "Move effect")
    public:
        MoveEffect(
                const Effect::ProcessModel& model,
                 Id<Effect::EffectModel> id,
                 int new_pos);

        void undo() const override;
        void redo() const override;

    protected:
        void serializeImpl(DataStreamInput & s) const override;
        void deserializeImpl(DataStreamOutput & s) override;

    private:
        Path<Effect::ProcessModel> m_model;
        Id<Effect::EffectModel> m_id;
        int m_oldPos, m_newPos{};
};
}
}
