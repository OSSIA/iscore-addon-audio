#include "ConstraintComponent.hpp"
#include <iscore/tools/std/Algorithms.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <Loop/LoopProcessModel.hpp>
#include <boost/multi_index_container.hpp>
#include <Audio/SoundProcess/SoundProcessModel.hpp>
#include <Audio/ReturnProcess/ReturnProcessModel.hpp>
#include <Audio/SendProcess/SendProcessModel.hpp>
#include <Audio/EffectProcess/EffectProcessModel.hpp>
#include <Audio/MixProcess/MixProcessModel.hpp>
#include <Audio/AudioStreamEngine/Scenario/ScenarioComponent.hpp>
#include <Audio/AudioStreamEngine/Context.hpp>
#include <Audio/AudioStreamEngine/Scenario/ScenarioComponentFactory.hpp>
#include <Audio/AudioStreamEngine/Scenario/LoopComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/EffectComponentFactory.hpp>
#include <Audio/AudioStreamEngine/Audio/SoundComponentFactory.hpp>
#include <Audio/AudioStreamEngine/Audio/MixComponentFactory.hpp>
#include <Audio/AudioStreamEngine/Audio/EffectComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/ReturnComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/SendComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/SoundComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/MixComponent.hpp>
#include <Audio/AudioStreamEngine/Utility.hpp>
#include <Audio/AudioStreamEngine/GroupAudioStream.h>
namespace Audio
{
namespace AudioStreamEngine
{

static const double* zero() {
    static const double val = 0;
    return &val;
}
static const double* one() {
    static const double val = 1;
    return &val;
}

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
    con(constraint.duration, &Scenario::ConstraintDurations::executionSpeedChanged,
        this, [=] (double d) {
        if(d > 0.01)
            m_stretch = d;
    });
    if(constraint.duration.executionSpeed() > 0.01)
        m_stretch = constraint.duration.executionSpeed();
}

ConstraintComponent::~ConstraintComponent()
{
}


void ConstraintComponent::makeStream(const Context& player)
{
    auto& cst = m_baseComponent.constraint;
    if(cst.processes.empty())
    {
        // Silence
    }
    else
    {
        using namespace std;
        std::vector<AudioStream> inputStreams;

        auto mix = findMix();
        std::function<const double*(
                    const Id<Process::ProcessModel>&)> getTarget;
        if(mix)
        {
            getTarget = [=] (const auto& in) {
                return mix->mix_ptr(in);
            };
        }
        else
        {
            getTarget = [] (const auto&...) {
                return one();
            };
        }

        auto make_stream_impl = [&] (auto component, const auto& proc)
        {
            if(component->getStream())
            {
                auto channel = MakeChannelSound(
                                   component->getStream(),
                                   getTarget(proc.id()));
                inputStreams.push_back(channel);
            }
        };

        for(auto& proc_pair :  m_baseComponent.processes())
        {
            auto& proc = proc_pair.process;
            auto& comp = proc_pair.component;
            if(auto scenar = dynamic_cast<ScenarioComponent*>(&comp))
            {
                make_stream_impl(scenar, proc);
            }
            else if(auto loop = dynamic_cast<LoopComponent*>(&comp))
            {
                make_stream_impl(loop, proc);
            }
            else if(auto sound = dynamic_cast<SoundComponent*>(&comp))
            {
                make_stream_impl(sound, proc);
            }
            else if(auto sfx = dynamic_cast<EffectProcessComponent*>(&comp))
            {
                // TODO if there is only an effect and no input,
                // we should feed it at least some silence.
                make_stream_impl(sfx, proc);
            }
            else if(auto ret = dynamic_cast<ReturnComponent*>(&comp))
            {
                make_stream_impl(ret, proc);
            }
            else if(auto send = dynamic_cast<SendComponent*>(&comp))
            {
                AudioStream stream = send->getStream();
                if(stream)
                {
                    auto channel = MakeChannelSound(stream, zero());
                    inputStreams.push_back(channel);
                }
            }
        }

        m_stream = MixNStreams(inputStreams);
        return;

        // TODO this does not work :
        /*
        m_stream = MakePitchSchiftTimeStretchSound(
                    MixNStreams(soundStreams),
                    &m_shift,
                    &m_stretch);
        */
    }

    // Look for all the "contents" process :
    // - Scenario
    // - Loop
    // Look for all the "effects" process and apply them

    // Then look for the "Mix" process and do the mix
}

AudioStream ConstraintComponent::makeInputMix(
        const Id<Process::ProcessModel>& target)
{
    std::vector<AudioStream> inputStreams;

    // First fetch the type of the target.
    // If it's an Fx...
    // If it's a Send...

    // Then : if there is no Mix process, everything has a 1 volume.
    // Else we use the Mix information.
    auto mix = findMix();
    std::function<const double*(const Id<Process::ProcessModel>&, const Id<Process::ProcessModel>&)> getTarget;
    if(mix)
    {
        getTarget = [=] (const auto& in, const auto& target) {
            return mix->mix_ptr(in, target);
        };
    }
    else
    {
        getTarget = [] (const auto&...) {
            return one();
        };
    }

    auto make_stream_impl = [&] (auto component, const auto& proc)
    {
        ISCORE_ASSERT(component->getStream());
        auto channel = MakeChannelSound(
                           MakeReturn(component->getStream()),
                           getTarget(proc.process.id(), target));
        inputStreams.push_back(channel);
    };

    for(auto proc : processes())
    {
        if(auto scenar = dynamic_cast<ScenarioComponent*>(&proc.component))
        {
            make_stream_impl(scenar, proc);
        }
        else if(auto loop = dynamic_cast<LoopComponent*>(&proc.component))
        {
            make_stream_impl(loop, proc);
        }
        else if(auto sound = dynamic_cast<SoundComponent*>(&proc.component))
        {
            make_stream_impl(sound, proc);
        }
        else if(auto ret = dynamic_cast<ReturnComponent*>(&proc.component))
        {
            make_stream_impl(ret, proc);
        }
        // TODO loop...
    }

    return MixNStreams(inputStreams);

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

Mix::ProcessModel* ConstraintComponent::findMix() const
{
    auto it = find_if(processes(), [] (auto val) {
        return dynamic_cast<Mix::ProcessModel*>(&val.process);
    });
    return it != processes().end() ? static_cast<Mix::ProcessModel*>(&it->process) : nullptr;
}


}
}
