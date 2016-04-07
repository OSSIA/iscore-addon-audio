#include "ConstraintComponent.hpp"
#include <iscore/tools/std/Algorithms.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <Loop/LoopProcessModel.hpp>

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
        StartSound(player, sound, start);
        StopSound(player, sound, end);
    }
    else if(cst.processes.size() == 1)
    {
        // Still silence, we assume only mixer process
        auto sound = MakeNullSound(cst.duration.maxDuration().msec());
        StartSound(player, sound, start);
        StopSound(player, sound, end);
    }
    else
    {
        // MixProcess* mix_proc{};
        for(auto& proc : cst.processes)
        {
            if(auto scenar = dynamic_cast<Scenario::ScenarioModel*>(&proc))
            {
                //inputStreams.emplace_back(*scenar, CreateAudioStream(*scenar));
            }
            else if(auto loop = dynamic_cast<Loop::ProcessModel*>(&proc))
            {
                //inputStreams.emplace_back(*loop, CreateAudioStream(*loop));
            }
            /*
                    else if(auto fx = dynamic_cast<Audio::EffectsProcess*>(&proc))
                    {
                        inputStreams.emplace_back(*fx, CreateAudioStream(*fx));
                    }
                    else if(auto mix = dynamic_cast<Audio::MixProcess*>(&proc))
                    {
                        mix_proc = mix;
                    }
                    */
        }
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
