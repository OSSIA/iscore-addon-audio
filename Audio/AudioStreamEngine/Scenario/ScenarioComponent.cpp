#include "ScenarioComponent.hpp"
#include <Scenario/Process/Algorithms/Accessors.hpp>
#include <Audio/AudioStreamEngine/Utility.hpp>
#include <Audio/AudioStreamEngine/GroupAudioStream.h>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Document/TimeNode/Trigger/TriggerModel.hpp>
namespace Audio
{
namespace AudioStreamEngine
{

ScenarioComponent::ScenarioComponent(
        const Id<iscore::Component>& id,
        Scenario::ScenarioModel& scenario,
        ScenarioComponent::system_t& doc,
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

    m_groupPlayer = MakeGroupPlayer();

    for(const hierarchy_t::ConstraintPair& cst : m_hm.constraints())
    {
        auto sound = cst.component.getStream();
        if(!sound)
            continue;

        auto start_date = GenSymbolicDate(m_groupPlayer);
        auto start_con = con(cst.element, &Scenario::ConstraintModel::executionStarted,
                           this, [&] () {
            onStartDateFixed(cst.component, GetAudioPlayerDateInFrame(m_groupPlayer));
        }, Qt::DirectConnection);
        m_synchros.insert(std::make_pair(cst.element.id(), std::make_pair(start_date, start_con)));

        auto stop_date = GenSymbolicDate(m_groupPlayer);
        auto stop_con = con(cst.element, &Scenario::ConstraintModel::executionStopped,
                                 this, [=] () {
            onStopDateFixed(cst.component, GetAudioPlayerDateInFrame(m_groupPlayer));
        }, Qt::DirectConnection);
        m_synchros.insert(std::make_pair(cst.element.id(), std::make_pair(stop_date, stop_con)));

        m_csts.insert(
                    std::make_pair(
                        cst.element.id(),
                        sound
                        )
                    );

        StartSound(m_groupPlayer, sound, start_date);
        StopSound(m_groupPlayer, sound, stop_date);

        cst.component.startDate = start_date;
        cst.component.stopDate = stop_date;
    }

    // We already set stuff starting from the initial events.
    if(!m_hm.scenario.startTimeNode().trigger()->active())
    {
        auto& start_node_id = m_hm.scenario.startTimeNode().id();

        auto it = find_if(m_hm.timeNodes(), [=] (auto& p) { return p.element.id() == start_node_id; });
        ISCORE_ASSERT(it != m_hm.timeNodes().end());
        onDateFixed(it->component, audio_frame_t{0});

    }

    m_stream = MakeSend(MakeGroupStream(m_groupPlayer));
}

template<>
ConstraintComponent* ScenarioComponent::make<ConstraintComponent, Scenario::ConstraintModel>(
        const Id<iscore::Component>& id,
        Scenario::ConstraintModel& elt,
        ScenarioComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent)
{
    auto comp = new ConstraintComponent{id, elt, doc, ctx, parent};
    comp->onStartDateFixed = [=] (audio_frame_t t) { onStartDateFixed(*comp, t); };
    comp->onStopDateFixed = [=] (audio_frame_t t) { onStopDateFixed(*comp, t); };
    return comp;
}

template<>
EventComponent* ScenarioComponent::make<EventComponent, Scenario::EventModel>(
        const Id<iscore::Component>& id,
        Scenario::EventModel& elt,
        ScenarioComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent)
{
    auto comp = new EventComponent{id, elt, doc, ctx, parent};
    comp->onDateFixed = [=] (audio_frame_t t) { onDateFixed(*comp, t); };
    return comp;
}

template<>
TimeNodeComponent* ScenarioComponent::make<TimeNodeComponent, Scenario::TimeNodeModel>(
        const Id<iscore::Component>& id,
        Scenario::TimeNodeModel& elt,
        ScenarioComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent)
{
    auto comp = new TimeNodeComponent{id, elt, doc, ctx, parent};
    comp->onDateFixed = [=] (audio_frame_t t) { onDateFixed(*comp, t); };
    return comp;
}

template<>
StateComponent* ScenarioComponent::make<StateComponent, Scenario::StateModel>(
        const Id<iscore::Component>& id,
        Scenario::StateModel& elt,
        ScenarioComponent::system_t& doc,
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

void ScenarioComponent::onDateFixed(const EventComponent& c, audio_frame_t time)
{
    auto& states = c.event.states();
    for(auto& st_id : states)
    {
        Scenario::StateModel& st = m_hm.scenario.states.at(st_id);
        if(auto& prev_cst_id = st.previousConstraint())
        {
            auto it = find_if(m_hm.constraints(),
                              [=] (auto& e) { return e.element.id() == prev_cst_id; });
            ISCORE_ASSERT(it != m_hm.constraints().end());
            it->component.onStopDateFixed(time);
        }

        if(auto& next_cst_id = st.nextConstraint())
        {
            auto it = find_if(m_hm.constraints(),
                              [=] (auto& e) { return e.element.id() == next_cst_id; });
            ISCORE_ASSERT(it != m_hm.constraints().end());
            it->component.onStartDateFixed(time + 1); // ** pay attention to the +1 **
        }
    }
}

void ScenarioComponent::onStartDateFixed(const ConstraintComponent& c, audio_frame_t time)
{
    if(!c.startDate)
        return;
    if(GetSymbolicDate(m_groupPlayer, c.startDate) != INT64_MAX)
        return; // this branch is already set.

    SetSymbolicDate(m_groupPlayer, c.startDate, time);

    const Scenario::ConstraintDurations& dur = c.constraint().duration;
    if(!dur.isRigid())
        return;

    auto end_date = time + toFrame(dur.defaultDuration());
    SetSymbolicDate(m_groupPlayer, c.stopDate, end_date);

    const Scenario::TimeNodeModel& end_tn = Scenario::endTimeNode(c.constraint(), m_hm.scenario);
    if(end_tn.trigger()->active())
        return;

    auto& end_tn_id = end_tn.id();

    auto it = find_if(m_hm.timeNodes(),
                      [=] (auto& e) { return e.element.id() == end_tn_id; });
    ISCORE_ASSERT(it != m_hm.timeNodes().end());
    it->component.onDateFixed(end_date);
}

void ScenarioComponent::onStopDateFixed(const ConstraintComponent& c, audio_frame_t time)
{
    if(!c.stopDate)
        return;
    if(GetSymbolicDate(m_groupPlayer, c.stopDate) != INT64_MAX)
        return; // this branch is already set.

    SetSymbolicDate(m_groupPlayer, c.stopDate, time);

    // We don't have anything more to do (it is only called to end interactive constraints.
}

audio_frame_t ScenarioComponent::toFrame(const TimeValue& t) const
{
    return t.msec() * m_hm.system.context.audio.sample_rate / 1000.0;
}

void ScenarioComponent::onDateFixed(const TimeNodeComponent& c, audio_frame_t time)
{
    // When a time node is set, this should ensure that we also recursively set
    // most following constraints / events.
    // A timenode is a stop if it has a trigger.
    // An event is a stop if it has a condition.
    // Note : preserve sample-accuracy for conditions by
    // keeping the stuff but muting it in some way. How to do this in a sample-acurate way ?
    // The only correct way is to evaluate the expression of an event in the audio "loop"...


    // TODO precompute dependency chains with boost.graph ?

    // The date of this time node has been fixed as 't'.
    // For each following constraint, we set :
    //  start = t + 1
    //  end = start + dur(cst)
    // Nothing is triggered however.

    auto& next_ev = c.timeNode.events();
    for(auto& ev_id : next_ev)
    {
        Scenario::EventModel& ev = m_hm.scenario.events.at(ev_id);
        if(!ev.condition().hasChildren())
        {
            auto it = find_if(m_hm.events(), [=] (auto& e) { return e.element.id() == ev_id; });
            ISCORE_ASSERT(it != m_hm.events().end());
            it->component.onDateFixed(time);
        }
    }
}

}
}
