#pragma once
#include <iscore/command/SerializableCommand.hpp>
#include <Audio/Commands/AudioCommandFactory.hpp>
#include <iscore/tools/ModelPath.hpp>
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
class UpdateMix final : public iscore::SerializableCommand
{
           ISCORE_COMMAND_DECL(Audio::CommandFactoryName(), UpdateMix, "Update mix")
    public:
        UpdateMix(
                const Mix::ProcessModel& model,
                Mix::Routing newmix);

        void undo() const override;
        void redo() const override;

    protected:
        void serializeImpl(DataStreamInput & s) const override;
        void deserializeImpl(DataStreamOutput & s) override;

    private:
        Path<Mix::ProcessModel> m_model;
        Mix::Routing m_old, m_new;

};
}
}
