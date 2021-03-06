#pragma once
#include <score/command/Command.hpp>
#include <Audio/Commands/AudioCommandFactory.hpp>
#include <score/model/path/Path.hpp>
#include <Audio/MixProcess/Mix.hpp>

namespace Audio
{
namespace Effect {
class EffectModel;
class ProcessModel;
class EffectFactory;
}
namespace Mix
{
class ProcessModel;
}

namespace Commands
{
class UpdateRouting final : public score::Command
{
           SCORE_COMMAND_DECL(Audio::CommandFactoryName(), UpdateRouting, "Update routing")
    public:
        UpdateRouting(
                const Mix::ProcessModel& model,
                Mix::Routing newmix);

        void undo(const score::DocumentContext& ctx) const override;
        void redo(const score::DocumentContext& ctx) const override;

        void update(
                const Mix::ProcessModel&,
                Mix::Routing newmix)
        {
            m_new = newmix;
        }

    protected:
        void serializeImpl(DataStreamInput & s) const override;
        void deserializeImpl(DataStreamOutput & s) override;

    private:
        Path<Mix::ProcessModel> m_model;
        Mix::Routing m_old, m_new;
};

class UpdateDirect final : public score::Command
{
           SCORE_COMMAND_DECL(Audio::CommandFactoryName(), UpdateDirect, "Update direct")
    public:
        UpdateDirect(
                const Mix::ProcessModel& model,
                Mix::DirectMix newmix);

        void undo(const score::DocumentContext& ctx) const override;
        void redo(const score::DocumentContext& ctx) const override;

        void update(
                const Mix::ProcessModel&,
                Mix::DirectMix newmix)
        {
            m_new = newmix;
        }

    protected:
        void serializeImpl(DataStreamInput & s) const override;
        void deserializeImpl(DataStreamOutput & s) override;

    private:
        Path<Mix::ProcessModel> m_model;
        Mix::DirectMix m_old, m_new;
};
}
}
