#include "EffectComponent.hpp"
#include <Audio/EffectProcess/EffectProcessModel.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Audio/AudioStreamEngine/Scenario/ConstraintComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/ScenarioComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/LoopComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/SoundComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/ReturnComponent.hpp>
#include <Audio/AudioStreamEngine/GroupAudioStream.h>
#include <Audio/AudioStreamEngine/Utility.hpp>

#include <Audio/EffectProcess/FaustEffectModel.hpp>
#include <Audio/EffectProcess/LocalTree/LocalTreeEffectProcessComponent.hpp>
#include <Audio/EffectProcess/LocalTree/LocalTreeEffectComponent.hpp>

namespace Audio
{
namespace AudioStreamEngine
{

EffectProcessComponent::EffectProcessComponent(
        const Id<iscore::Component>& id,
        Effect::ProcessModel& sound,
        const EffectProcessComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent_obj):
    ProcessComponent_T{sound, id, "EffectComponent", parent_obj}
{

}

void EffectProcessComponent::makeStream(const Context& ctx)
{
    m_effects.clear();

    // For all "generative" streams in the parent constraint,
    // take their "send" streams, create returns, mix the returns,
    // put it on input of the effect, create a send, return the output
    auto parent_cst = safe_cast<Scenario::ConstraintModel*>(process().parent());

    // Get its audio component
    auto cst_comp_it = find_if(
                parent_cst->components,
                [] (auto& comp) {
        return dynamic_cast<ConstraintComponent*>(&comp);
    });

    // The constraint has the mix information, hence we request it to create
    // the mix.
    if(cst_comp_it == parent_cst->components.end())
        return;
    auto cst_comp = static_cast<ConstraintComponent*>(&(*cst_comp_it));
    AudioStream sound = cst_comp->makeInputMix(this->process().id());

    ISCORE_ASSERT(sound);
    for(auto& fx : process().effects())
    {
        if(auto faust_fx = dynamic_cast<Effect::FaustEffectModel*>(&fx))
        {
            auto compiled_fx = faust_fx->effect();
            if(!compiled_fx)
                continue;

            sound = MakeEffectSound(sound, compiled_fx, 0, 0);
            ISCORE_ASSERT(sound);
            m_effects.insert(std::make_pair(fx.id(), compiled_fx));


            // Find local tree component.
            auto it = find_if(faust_fx->components, [] (const auto& comp) {
                return dynamic_cast<const Audio::Effect::LocalTree::FaustComponent*>(&comp);
            });
            if(it != faust_fx->components.end())
            {
                static_cast<Audio::Effect::LocalTree::FaustComponent&>(*it).m_audio_effect = compiled_fx;
            }
        }
    }

    m_stream = MakeSend(sound);
}

}
}
