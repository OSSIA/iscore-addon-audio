#include "ConstraintComponent.hpp"
#include <ossia/detail/algorithms.hpp>
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
#include <Audio/AudioStreamEngine/Scenario/ScenarioComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/LoopComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/EffectComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/InputComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/ReturnComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/SendComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/SoundComponent.hpp>
#include <Audio/AudioStreamEngine/Utility.hpp>
#include <Audio/AudioStreamEngine/Streams/AudioStreamIScoreExtensions.h>
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

ConstraintBase::ConstraintBase(
        Scenario::ConstraintModel& constraint,
        ConstraintBase::system_t& doc,
        const Id<iscore::Component>& id,
        QObject* parent_comp):
    Scenario::ConstraintComponent<Component>{constraint, doc, id, "ConstraintComponent", parent_comp}
{
}

Constraint::Constraint(
        Scenario::ConstraintModel& constraint,
        ConstraintBase::system_t& doc,
        const Id<iscore::Component>& id,
        QObject* parent_comp):
    parent_t{constraint, doc, id, parent_comp}
{
    con(constraint.duration, &Scenario::ConstraintDurations::executionSpeedChanged,
        this, [=] (double d) {
        if(d > 0.01)
            m_stretch = d;
    });
    if(constraint.duration.executionSpeed() > 0.01)
        m_stretch = constraint.duration.executionSpeed();
}

Constraint::~Constraint()
{
}

optional<AudioGraphVertice> Constraint::visit(AudioGraph& graph)
{
    int n_proc = this->children().size();
    std::vector<AudioGraphVertice> processes;
    processes.reserve(n_proc);
    std::vector<AudioGraphVertice> generators; // Everything that output a mixable stream
    generators.reserve(n_proc);
    std::vector<AudioGraphVertice> inputs; // FX, Send
    inputs.reserve(n_proc / 2);

    // If there is no mix process, we assume that everything is
    // routed into everything.
    // Else, we use it to create the correct relations.
    // TODO think of the policies !!!!
    for(auto& proc_pair : this->children())
    {
        ProcessComponent& proc = *proc_pair.component;
        auto maybe_sub_res = proc.visit(graph);

        if(maybe_sub_res)
        {
            auto& sub_res = *maybe_sub_res;
            if(proc.hasInput())
            {
                inputs.push_back(sub_res);
            }

            if(proc.hasOutput())
            {
                generators.push_back(sub_res);
            }

            processes.push_back(sub_res);
        }
    }

    // For all inputs, generate edges going from all generators
    const auto mix = findMix();
    for(auto input : inputs)
    {
        for(auto gen : generators)
        {
            if(gen != input)
            {
                if(mix)
                {
                    auto gen_ptr = safe_cast<ProcessComponent*>(graph[gen]);
                    auto in_ptr = safe_cast<ProcessComponent*>(graph[input]);

                    auto gen_id = gen_ptr->process().id();
                    auto in_id = in_ptr->process().id();

                    auto routing_it = mix->routings().find(Mix::Routing{gen_id, in_id});
                    if(routing_it == mix->routings().end())
                    {
                        qDebug() << "routing not found in mix";
                    }
                    else if(routing_it->enabled)
                    {
                        boost::add_edge(gen, input, graph);
                    }
                }
                else
                {
                    boost::add_edge(gen, input, graph);
                }
            }
        }
    }

    if(!processes.empty())
    {
        auto res = boost::add_vertex(this, graph);

        // Add edge from all processes to the constraint mix stream
        // Even for the sends : the constraint "pulls" the send process
        // to ensure that it is up-to-date.
        for(auto proc : processes)
        {
            boost::add_edge(proc, res, graph);
        }

        return res;
    }

    return {};
}

ConstraintBase::~ConstraintBase()
{
}


void Constraint::makeStream(const Context& player)
{
    if(constraint().processes.empty())
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

        auto make_stream_impl = [&] (auto& component, const auto& proc)
        {
            auto stream = component.getStream();
            if(stream)
            {
                auto dur = GetLengthSound(stream);
                auto& cst_dur = constraint().duration;

                AudioStream extended_stream;
                TimeValue def = cst_dur.defaultDuration();
                TimeValue maxdur = cst_dur.maxDuration();

                audio_frame_t parent_max_dur =
                    cst_dur.isMaxInfinite()
                        ? INT64_MAX
                        : system().toFrame(cst_dur.isRigid() ? def : maxdur);
                if(dur >= 0 && dur < parent_max_dur)
                {
                    // TODO this may break with rubberband...
                    extended_stream =
                            MakeSeqSound(
                                stream,
                                MakeMultiNullSound(
                                    GetChannelsSound(stream),
                                    parent_max_dur - dur),
                                0);
                }
                else
                {
                    extended_stream = stream;
                }

                auto channel = MakeChannelSound(
                                   extended_stream,
                                   getTarget(proc.id()));
                inputStreams.push_back(channel);
            }
        };

        for(auto& proc_pair : this->children())
        {
            auto& proc = *proc_pair.model;
            auto& comp = *proc_pair.component;
            if(auto send = dynamic_cast<SendComponent*>(&comp))
            {
                AudioStream stream = send->getStream();
                if(stream)
                {
                    // We disable the sound output of the sends, we only pull them regularly.
                    // This would maybe be better achieved with a regular command.
                    auto channel = MakeChannelSound(stream, zero());
                    inputStreams.push_back(channel);
                }
            }
            else if(auto input = dynamic_cast<InputComponent*>(&comp))
            {
                inputStreams.push_back(input->getStream());
            }
            else if(comp.hasOutput())
            {
                // TODO for the effects case, if there is only an effect and no input,
                // we should feed it at least some silence.
                make_stream_impl(comp, proc);
            }
        }

        m_stream = MixNStreams(inputStreams);
        if(!realTime())
        {
            m_stream = MakePitchSchiftTimeStretchSound(
                        m_stream,
                        &m_shift,
                        &m_stretch);
        }
        m_stream = MakeFadeSound(m_stream, 100, 100);
    }

    // Look for all the "contents" process :
    // - Scenario
    // - Loop
    // Look for all the "effects" process and apply them

    // Then look for the "Mix" process and do the mix
}

AudioStream Constraint::makeInputMix(
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

    auto make_stream_impl = [&] (const auto& pair)
    {
        ISCORE_ASSERT(pair.component->getStream());
        auto channel = MakeChannelSound(
                           MakeReturn(pair.component->getStream()),
                           getTarget(pair.model->id(), target));
        ISCORE_ASSERT(channel);
        inputStreams.push_back(channel);
    };

    ISCORE_ASSERT(children().size() > 0);

    for(auto pair : children())
    {
        if(pair.model->id() == target)
            continue;

        if(mix)
        {
            auto routing_it = mix->routings().find(Mix::Routing{pair.model->id(), target});
            if(routing_it == mix->routings().end())
            {
                qDebug() << "Mix not found !" << pair.model->id() << target;
            }
            else if(!routing_it->enabled)
            {
                continue;
            }
        }
        else
        {
            // We disable effect -> effect when there is no mix.
            // the better way would be to automatically get a mix component on "audio" constraints...
            // but this needs serializable components...
            // Find target
            auto target_proc_it = ossia::find_if(children(),
                                          [=] (const auto& p) {
                return p.model->id() == target;
            });
            ISCORE_ASSERT(target_proc_it != children().end());
            auto& target_comp = target_proc_it->component;
            if(pair.component->hasInput() && pair.component->hasOutput() &&
               target_comp->hasInput() && target_comp->hasOutput())
            {
                continue;
            }
        }

        // TODO this does not work if there are two effect chains :
        // they will go into each other.
        // Use the GUI disablement.
        if(pair.component->hasOutput())
        {
            make_stream_impl(pair);
        }
    }

    return MixNStreams(inputStreams);
}


ProcessComponent* ConstraintBase::make(
        const Id<iscore::Component>& id,
        ProcessComponentFactory& factory,
        Process::ProcessModel& process)
{
    return factory.make(process, system(), id, this);
}


void ConstraintBase::removing(
        const Process::ProcessModel& cst,
        const ProcessComponent& comp)
{
}

Mix::ProcessModel* Constraint::findMix() const
{
    auto& procs = constraint().processes;
    auto it = ossia::find_if(procs, [] (auto& val) {
        return dynamic_cast<Mix::ProcessModel*>(&val);
    });
    return it != procs.end() ? static_cast<Mix::ProcessModel*>(&(*it)) : nullptr;
}
}
}
