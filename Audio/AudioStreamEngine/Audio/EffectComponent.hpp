#pragma once
#include <Audio/AudioStreamEngine/Scenario/ProcessComponent.hpp>

namespace Audio
{
namespace Effect
{
class ProcessModel;
}
namespace AudioStreamEngine
{
class EffectComponent final : public ProcessComponent_T<Effect::ProcessModel>
{
       COMPONENT_METADATA(Audio::AudioStreamEngine::EffectComponent)

        using system_t = Audio::AudioStreamEngine::DocumentPlugin;
    public:
       EffectComponent(
               const Id<Component>& id,
               Effect::ProcessModel& sound,
               const system_t& doc,
               const iscore::DocumentContext& ctx,
               QObject* parent_obj);


       AudioStream makeStream(const Context& ctx) const override;
};
}
}
