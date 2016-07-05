#pragma once
#include <Audio/AudioStreamEngine/Scenario/ProcessComponent.hpp>

namespace Audio
{
namespace Effect
{
class ProcessModel;
class EffectModel;
}
namespace AudioStreamEngine
{
class EffectProcessComponent final :
        public ProcessComponent_T<Effect::ProcessModel, true, true>
{
       COMPONENT_METADATA(Audio::AudioStreamEngine::EffectProcessComponent)

        using system_t = Audio::AudioStreamEngine::DocumentPlugin;
    public:
       EffectProcessComponent(
               const Id<Component>& id,
               Effect::ProcessModel& sound,
               const system_t& doc,
               const iscore::DocumentContext& ctx,
               QObject* parent_obj);


       void makeStream(const Context& ctx) override;

       AudioEffect effect(const Id<Effect::EffectModel>& id) const
       { return m_effects.at(id); }
    private:
       std::map<Id<Effect::EffectModel>, AudioEffect> m_effects;
};

AUDIO_COMPONENT_FACTORY(EffectProcessComponentFactory, "ebfdbbb4-9f19-4b64-bc92-374af65122d3", EffectProcessComponent, Effect::ProcessModel)
}
}
