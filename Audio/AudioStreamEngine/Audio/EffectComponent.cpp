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

namespace Audio
{
namespace AudioStreamEngine
{

EffectComponent::EffectComponent(
        const Id<iscore::Component>& id,
        Effect::ProcessModel& sound,
        const EffectComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent_obj):
    ProcessComponent_T{sound, id, "EffectComponent", parent_obj}
{

}

void EffectComponent::makeStream(const Context& ctx)
{
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

    std::vector<AudioStream> inputStreams;
    if(cst_comp_it != parent_cst->components.end())
    {
        auto cst_comp = static_cast<ConstraintComponent*>(&(*cst_comp_it));

        for(auto proc : cst_comp->processes())
        {
            if(auto scenar = dynamic_cast<ScenarioComponent*>(&proc.component))
            {
                inputStreams.push_back(scenar->getStream());
            }
            else if(auto sound = dynamic_cast<SoundComponent*>(&proc.component))
            {
                inputStreams.push_back(sound->getStream());
            }
            else if(auto sound = dynamic_cast<ReturnComponent*>(&proc.component))
            {
                inputStreams.push_back(sound->getStream());
            }
            // TODO loop...
        }
    }

    std::vector<AudioStream> returns;
    transform(inputStreams, std::back_inserter(returns), [] (auto stream) {
        return MakeReturn(stream);
    });

    auto mixed = MixNStreams(returns);
    //m_stream = mixed;
    auto fx = MakeFaustAudioEffect("/tmp/examples/freeverb.dsp", "", "");
    auto snd_fx = MakeEffectSound(mixed, fx, 0, 0);
    m_stream = MakeSend(snd_fx);
}

}
}
