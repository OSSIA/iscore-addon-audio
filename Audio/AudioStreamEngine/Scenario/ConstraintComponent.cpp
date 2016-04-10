#include "ConstraintComponent.hpp"
#include <iscore/tools/std/Algorithms.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <Loop/LoopProcessModel.hpp>
#include <boost/multi_index_container.hpp>
#include <Audio/SoundProcess/SoundProcessModel.hpp>
#include <Audio/EffectProcess/EffectProcessModel.hpp>
#include <Audio/MixProcess/MixProcessModel.hpp>
#include <Audio/AudioStreamEngine/Scenario/ScenarioComponent.hpp>
#include <Audio/AudioStreamEngine/Context.hpp>
#include <Audio/AudioStreamEngine/Scenario/ScenarioComponentFactory.hpp>
#include <Audio/AudioStreamEngine/Audio/EffectComponentFactory.hpp>
#include <Audio/AudioStreamEngine/Audio/SoundComponentFactory.hpp>
#include <Audio/AudioStreamEngine/Audio/MixComponentFactory.hpp>
#include <Audio/AudioStreamEngine/Audio/EffectComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/SoundComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/MixComponent.hpp>
#include <Audio/AudioStreamEngine/Utility.hpp>
namespace Audio
{
namespace AudioStreamEngine
{

const iscore::Component::Key&ConstraintComponent::key() const
{
    static const Key k{"a19b8885-5077-4331-a6b1-828571fa5128"};
    return k;
}


ConstraintComponent::ConstraintComponent(
        const Id<iscore::Component>& id,
        Scenario::ConstraintModel& constraint,
        const ConstraintComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent_comp):
    Component{id, "ConstraintComponent", parent_comp},
    m_baseComponent{*this, constraint, doc, ctx, this}
{
}

ConstraintComponent::~ConstraintComponent()
{
}


AudioStream ConstraintComponent::makeStream(
        const Context& player,
        SymbolicDate start,
        SymbolicDate end)
{

    auto& cst = m_baseComponent.constraint;
    if(cst.processes.empty())
    {
        // Silence
        auto sound = MakeNullSound(cst.duration.maxDuration().msec());
        StartSound(player.audio.player, sound, start);
        StopSound(player.audio.player, sound, end);
    }
    else
    {
        using namespace std;
        // Input mix
        map<Id<Process::ProcessModel>, pair<Scenario::ScenarioModel*, ScenarioComponent*>> scenarios;
        //map<Id<Scenario::ScenarioModel>, Loop::ProcessModel*> loops;
        map<Id<Process::ProcessModel>, pair<Sound::ProcessModel*, SoundComponent*>> sounds;
        map<Id<Process::ProcessModel>, pair<Effect::ProcessModel*, EffectComponent*>> sfxs;
        map<Id<Process::ProcessModel>, pair<Mix::ProcessModel*, MixComponent*>> mixs;

        for(auto& proc_pair :  m_baseComponent.processes())
        {
            auto& proc = proc_pair.process;
            auto& comp = proc_pair.component;
            if(auto scenar = dynamic_cast<Scenario::ScenarioModel*>(&proc))
            {
                pair<Scenario::ScenarioModel*, ScenarioComponent*> f = make_pair(scenar, safe_cast<ScenarioComponent*>(&comp));
                scenarios.insert(make_pair(scenar->id(), f));
            }
            else if(auto loop = dynamic_cast<Loop::ProcessModel*>(&proc))
            {
                //loops.insert(loop->id(), loop);
            }
            else if(auto sound = dynamic_cast<Sound::ProcessModel*>(&proc))
            {
                sounds.insert(make_pair(sound->id(), make_pair(sound, safe_cast<SoundComponent*>(&comp))));
            }
            else if(auto sfx = dynamic_cast<Effect::ProcessModel*>(&proc))
            {
                sfxs.insert(make_pair(sfx->id(), make_pair(sfx, safe_cast<EffectComponent*>(&comp))));
            }
            else if(auto mix = dynamic_cast<Mix::ProcessModel*>(&proc))
            {
                mixs.insert(make_pair(mix->id(), make_pair(mix, safe_cast<MixComponent*>(&comp))));
            }
        }
        // For now we just put all the sound streams in parallel...
        std::vector<AudioStream> soundStreams;
        for(auto sound : sounds)
        {
            auto stream = sound.second.second->makeStream(player);
            if(stream)
                soundStreams.push_back(stream);
        }
        for(auto scenario : scenarios)
        {
            auto stream = scenario.second.second->makeStream(player);
            if(stream)
                soundStreams.push_back(stream);
        }
        return makeNStreamsParallel(soundStreams);
    }

    // Look for all the "contents" process :
    // - Scenario
    // - Loop
    // Look for all the "effects" process and apply them

    // Then look for the "Mix" process and do the mix

    return {};
}


ProcessComponent*ConstraintComponent::make_processComponent(
        const Id<iscore::Component>& id,
        ProcessComponentFactory& factory,
        Process::ProcessModel& process,
        const DocumentPlugin& system,
        const iscore::DocumentContext& ctx,
        QObject* parent_component)
{
    return factory.make(id, process, system, ctx, parent_component);
}


void ConstraintComponent::removing(
        const Process::ProcessModel& cst,
        const ProcessComponent& comp)
{
}


}
}
