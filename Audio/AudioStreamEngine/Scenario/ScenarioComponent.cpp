#include "ScenarioComponent.hpp"
#include <Scenario/Process/Algorithms/Accessors.hpp>
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
    ProcessComponent{scenario, id, "ScenarioComponent", parent_obj},
    m_hm{*this, scenario, doc, ctx, this}
{
}

AudioStream ScenarioComponent::CreateAudioStream(const Context& ctx)
{
    auto& scenario = m_hm.scenario;
    std::map<Id<Scenario::TimeNodeModel>, SymbolicDate> synchros;
    std::map<Id<Scenario::ConstraintModel>, AudioStream> csts;
    // First generate a symbolic date for each of the timenode (fixed if there is no trigger ?)
    for(auto& tn : scenario.timeNodes)
    {
        // TODO right now audioplayer is not used in GenSymbolicDate but this may be dangerous...
        synchros.insert(std::make_pair(tn.id(), GenSymbolicDate(AudioPlayerPtr{})));
    }

    // Then create a stream for all the constraints
    // and set the start / end of each parent stream of the constraint
    // to the symbolic date of the trigger
    for(const hierarchy_t::ConstraintPair& cst : m_hm.constraints())
    {
        // Optimize me by storing the time node ids beforehand.
        csts.insert(
                    std::make_pair(
                        cst.element.id(),
                        cst.component.makeStream(
                            ctx,
                            synchros.at(Scenario::startEvent(cst.element, scenario).timeNode()),
                            synchros.at(Scenario::endEvent(cst.element, scenario).timeNode())
                            )
                        )
                    );
    }


    return {};
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
