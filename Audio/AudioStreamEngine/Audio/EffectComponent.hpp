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
       COMPONENT_METADATA("1b7abce0-47d0-4f39-96f3-6308445be35e")

    public:
       EffectProcessComponent(
               Effect::ProcessModel& sound,
               DocumentPlugin& doc,
               const Id<iscore::Component>& id,
               QObject* parent_obj);

       void makeStream(const Context& ctx) override;

       AudioEffect effect(const Id<Effect::EffectModel>& id) const
       { return m_effects.at(id); }
    private:
       std::map<Id<Effect::EffectModel>, AudioEffect> m_effects;
};

using EffectProcessComponentFactory = ProcessComponentFactory_T<EffectProcessComponent>;
}
}
