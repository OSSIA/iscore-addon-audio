#include "ScenarioComponent.hpp"
#include <Scenario/Process/Algorithms/Accessors.hpp>
#include <Audio/AudioStreamEngine/Utility.hpp>
namespace Audio
{
namespace AudioStreamEngine
{

ScenarioComponent::ScenarioComponent(
        const Id<iscore::Component>& id,
        Scenario::ScenarioModel& scenario,
        const ScenarioComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent_obj):
    ProcessComponent_T{scenario, id, "ScenarioComponent", parent_obj},
    m_hm{*this, scenario, doc, ctx, this}
{
}

AudioStream ScenarioComponent::makeStream(const Context& ctx) const
{
    for(auto& elt : m_synchros)
    {
        QObject::disconnect(elt.second.second);
    }
    m_synchros.clear();
    m_csts.clear();

    auto& scenario = process();
    // First generate a symbolic date for each of the timenode (fixed if there is no trigger ?)
    for(Scenario::TimeNodeModel& tn : scenario.timeNodes)
    {
        auto date = GenSymbolicDate(ctx.audio.player);
        auto con = connect(&tn, &Scenario::TimeNodeModel::triggeredByEngine,
                           this, [=, &ctx] () {
            SetSymbolicDate(ctx.audio.player, date, GetAudioPlayerDateInFrame(ctx.audio.player));
        });
        m_synchros.insert(std::make_pair(tn.id(), std::make_pair(date, con)));
    }

    // Then create a stream for all the constraints
    // and set the start / end of each parent stream of the constraint
    // to the symbolic date of the trigger
    for(const hierarchy_t::ConstraintPair& cst : m_hm.constraints())
    {
        // Optimize me by storing the time node ids beforehand.
        m_csts.insert(
                    std::make_pair(
                        cst.element.id(),
                        cst.component.makeStream(
                            ctx,
                            m_synchros.at(Scenario::startEvent(cst.element, scenario).timeNode()).first,
                            m_synchros.at(Scenario::endEvent(cst.element, scenario).timeNode()).first
                            )
                        )
                    );
    }

    // We put our constraint sounds in parallel
    std::vector<AudioStream> vec;
    for(auto cst : m_csts)
    {
        vec.push_back(cst.second);
    }
    return makeNStreamsParallel(vec);
}

template<>
ConstraintComponent* ScenarioComponent::make<ConstraintComponent, Scenario::ConstraintModel>(
        const Id<iscore::Component>& id,
        Scenario::ConstraintModel& elt,
        const ScenarioComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent)
{
    return new ConstraintComponent{id, elt, doc, ctx, parent};
}

template<>
EventComponent* ScenarioComponent::make<EventComponent, Scenario::EventModel>(
        const Id<iscore::Component>& id,
        Scenario::EventModel& elt,
        const ScenarioComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent)
{
    return new EventComponent{id, elt, doc, ctx, parent};
}

template<>
TimeNodeComponent* ScenarioComponent::make<TimeNodeComponent, Scenario::TimeNodeModel>(
        const Id<iscore::Component>& id,
        Scenario::TimeNodeModel& elt,
        const ScenarioComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent)
{
    return new TimeNodeComponent{id, elt, doc, ctx, parent};
}

template<>
StateComponent* ScenarioComponent::make<StateComponent, Scenario::StateModel>(
        const Id<iscore::Component>& id,
        Scenario::StateModel& elt,
        const ScenarioComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent)
{
    return new StateComponent{id, elt, doc, ctx, parent};
}

void ScenarioComponent::removing(const Scenario::ConstraintModel& elt, const ConstraintComponent& comp)
{
}

void ScenarioComponent::removing(const Scenario::EventModel& elt, const EventComponent& comp)
{
}

void ScenarioComponent::removing(const Scenario::TimeNodeModel& elt, const TimeNodeComponent& comp)
{
}

void ScenarioComponent::removing(const Scenario::StateModel& elt, const StateComponent& comp)
{
}

}
}
