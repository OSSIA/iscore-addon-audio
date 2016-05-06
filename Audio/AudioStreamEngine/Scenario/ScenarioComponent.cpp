#include "ScenarioComponent.hpp"
#include <Scenario/Process/Algorithms/Accessors.hpp>
#include <Audio/AudioStreamEngine/Utility.hpp>
#include <Audio/AudioStreamEngine/GroupAudioStream.h>
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

void ScenarioComponent::makeStream(const Context& ctx)
{
    for(auto& elt : m_synchros)
    {
        QObject::disconnect(elt.second.second);
    }
    m_synchros.clear();
    m_csts.clear();

    m_renderer = MakeGroupPlayer();

    auto& scenario = process();

    // TODO FIXME we have to generate a symbolic date for each *condition*.
    // First generate a symbolic date for each of the timenode (fixed if there is no trigger ?)
    // TODO we should use directly the OSSIA execution components instead.
    // But for this we have to ensure that they exist
    // Hence we have to introduce a dependency graph for the construction of components trees.
    for(Scenario::TimeNodeModel& tn : scenario.timeNodes)
    {
        auto date = GenSymbolicDate(m_renderer);
        auto con = connect(&tn, &Scenario::TimeNodeModel::triggeredByEngine,
                           this, [=] () {
            qDebug() << SetSymbolicDate(m_renderer, date, GetAudioPlayerDateInFrame(m_renderer));
        }, Qt::QueuedConnection);
        m_synchros.insert(std::make_pair(tn.id(), std::make_pair(date, con)));
    }

    // Then create a stream for all the constraints
    // and set the start / end of each parent stream of the constraint
    // to the symbolic date of the trigger
    for(const hierarchy_t::ConstraintPair& cst : m_hm.constraints())
    {
        auto sound = cst.component.getStream();
        if(!sound)
            continue;

        // Optimize me by storing the time node ids beforehand.
        auto t_start = m_synchros.at(Scenario::startEvent(cst.element, scenario).timeNode()).first;
        auto t_end = m_synchros.at(Scenario::endEvent(cst.element, scenario).timeNode()).first;
        m_csts.insert(
                    std::make_pair(
                        cst.element.id(),
                        sound
                        )
                    );

        StartSound(m_renderer, sound, t_start);
        StopSound(m_renderer, sound, t_end);
    }

    m_stream = MakeSend(MakeGroupStream(m_renderer));
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
