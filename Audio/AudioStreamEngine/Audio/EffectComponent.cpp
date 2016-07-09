#include "EffectComponent.hpp"
#include <Audio/EffectProcess/EffectProcessModel.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Audio/AudioStreamEngine/Scenario/ConstraintComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/ScenarioComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/LoopComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/SoundComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/ReturnComponent.hpp>
#include <Audio/AudioStreamEngine/Streams/AudioStreamIScoreExtensions.h>
#include <Audio/AudioStreamEngine/Utility.hpp>

#include <Audio/EffectProcess/FaustEffectModel.hpp>
#include <Audio/EffectProcess/LocalTree/LocalTreeEffectProcessComponent.hpp>

namespace Audio
{
namespace AudioStreamEngine
{

EffectProcessComponent::EffectProcessComponent(
        const Id<iscore::Component>& id,
        Effect::ProcessModel& sound,
        const EffectProcessComponent::system_t& doc,
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
    auto& cst_comp = iscore::component<Constraint>(parent_cst->components);

    // The constraint has the mix information, hence we request it to create
    // the mix.
    AudioStream sound = cst_comp.makeInputMix(this->process().id());

    if(!sound)
    {
        // No input -> maybe the effect will still be making some noise
        // or act as an instrument ?
        sound = MakeNullSound(LONG_MAX);
    }

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

    if(sound)
        m_stream = MakeSend(sound);
}

}
}
