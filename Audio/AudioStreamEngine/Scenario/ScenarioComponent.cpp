#include "ScenarioComponent.hpp"
#include <Scenario/Process/Algorithms/Accessors.hpp>
#include <Audio/AudioStreamEngine/Utility.hpp>
#include <Audio/AudioStreamEngine/Streams/AudioStreamIScoreExtensions.h>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Document/TimeNode/Trigger/TriggerModel.hpp>
namespace Audio
{
namespace AudioStreamEngine
{

ScenarioComponentBase::ScenarioComponentBase(
        Scenario::ProcessModel& scenario,
        DocumentPlugin& doc,
        const Id<iscore::Component>& id,
        QObject* parent_obj):
    ProcessComponent_T{scenario, doc, id, "ScenarioComponent", parent_obj}
{
}

ScenarioComponent::~ScenarioComponent()
{
}

optional<AudioGraphVertice> ScenarioComponent::visit(AudioGraph& graph)
{
    auto res = boost::add_vertex(this, graph);

    for(auto& constraint : constraints_pairs())
    {
        if(auto cst_vtx = constraint.component.visit(graph))
        {
            boost::add_edge(*cst_vtx, res, graph);
        }
    }

    return res;
}

void ScenarioComponent::makeStream(const Context& ctx)
{
    for(auto& con : m_connections)
        QObject::disconnect(con);
    m_connections.clear();

    // Initialization of the streams
    for(auto& p : constraints_pairs())
    {
        p.component.onStartDateFixed = [&] (audio_frame_t t, bool force) { onStartDateFixed(p.component, t, force); };
        p.component.onStopDateFixed = [&] (audio_frame_t t) { onStopDateFixed(p.component, t); };
    }

    for(auto& p : events_pairs())
    {
        p.component.onDateFixed = [&] (audio_frame_t t, bool force) { onDateFixed(p.component, t, force); };
    }

    for(auto& p : timeNodes_pairs())
    {
        p.component.onDateFixed = [&] (audio_frame_t t, bool force) { onDateFixed(p.component, t, force); };
    }


    m_groupPlayer = MakeGroupPlayer();
    std::list<ConstraintPair> topo_sorted;

    for(const auto& cst : constraints_pairs())
    {
        auto sound = cst.component.getStream();
        if(!sound)
            continue;
        topo_sorted.push_back(cst);
    }

    topo_sorted.sort([] (const ConstraintPair& lhs, const ConstraintPair& rhs)
    {
         return lhs.component.priority < rhs.component.priority;
    });

    for(const auto& cst :topo_sorted)
    {
        auto sound = cst.component.getStream();
        {
            auto start_con = con(cst.element, &Scenario::ConstraintModel::executionStarted,
                                 this, [=] () {
                onStartDateFixed(cst.component, GetAudioPlayerDateInFrame(m_groupPlayer), true);
            }, Qt::DirectConnection);
            m_connections.push_back(start_con);
        }

        {
            auto stop_con = con(cst.element, &Scenario::ConstraintModel::executionStopped,
                                this, [=] () {
                onStopDateFixed(cst.component, GetAudioPlayerDateInFrame(m_groupPlayer));
            }, Qt::DirectConnection);

            m_connections.push_back(stop_con);
        }

        {
            auto speed_con = con(cst.element.duration, &Scenario::ConstraintDurations::executionSpeedChanged,
                                this, [=] (double s) {
                // The new end duration is the "base" end multiplied by the speed.
                onSpeedChanged(cst.component, s);
            }, Qt::QueuedConnection);

            m_connections.push_back(speed_con);
        }

        cst.component.startDate = GenPriorisedSymbolicDate(m_groupPlayer, cst.component.priority);
        cst.component.stopDate = GenPriorisedSymbolicDate(m_groupPlayer, cst.component.priority);
        StartSound(m_groupPlayer, sound, cst.component.startDate);
        StopSound(m_groupPlayer, sound, cst.component.stopDate);
    }

    // We already set stuff starting from the initial events.
    if(!process().startTimeNode().trigger()->active())
    {
        auto& start_node_id = process().startTimeNode().id();

        auto it = ossia::find_if(timeNodes_pairs(), [=] (auto& p) { return p.element.id() == start_node_id; });
        ISCORE_ASSERT(it != timeNodes_pairs().end());
        onDateFixed(it->component, audio_frame_t{0}, true);

    }

    m_stream = MakeSend(MakeGroupStream(m_groupPlayer));
}

void ScenarioComponent::stop()
{
  for(auto& constraint : constraints_pairs())
    constraint.component.stop();
}

template<>
Constraint* ScenarioComponentBase::make<Constraint, Scenario::ConstraintModel>(
    const Id<iscore::Component>& id,
    Scenario::ConstraintModel& elt)
{
    return new Constraint{elt, system(), id, this};
}

template<>
Event* ScenarioComponentBase::make<Event, Scenario::EventModel>(
        const Id<iscore::Component>& id,
        Scenario::EventModel& elt)
{
    return new Event{id, elt, system(), this};
}

template<>
TimeNode* ScenarioComponentBase::make<TimeNode, Scenario::TimeNodeModel>(
        const Id<iscore::Component>& id,
        Scenario::TimeNodeModel& elt)
{
    return new TimeNode{id, elt, system(), this};
}

template<>
State* ScenarioComponentBase::make<State, Scenario::StateModel>(
        const Id<iscore::Component>& id,
        Scenario::StateModel& elt)
{
    return nullptr;
}

void ScenarioComponent::onDateFixed(
        const Event& c,
        audio_frame_t time,
        bool force_update)
{
    auto& states = c.event.states();
    for(auto& st_id : states)
    {
        Scenario::StateModel& st = process().states.at(st_id);
        if(auto& prev_cst_id = st.previousConstraint())
        {
            auto it = ossia::find_if(constraints_pairs(),
                              [=] (auto& e) { return e.element.id() == prev_cst_id; });
            ISCORE_ASSERT(it != constraints_pairs().end());
            it->component.onStopDateFixed(time);
        }

        if(auto& next_cst_id = st.nextConstraint())
        {
            auto it = ossia::find_if(constraints_pairs(),
                              [=] (auto& e) { return e.element.id() == next_cst_id; });
            ISCORE_ASSERT(it != constraints_pairs().end());
            it->component.onStartDateFixed(time + 1, force_update); // ** pay attention to the +1 **
        }
    }
}

void ScenarioComponent::onStartDateFixed(
        Constraint& c,
        audio_frame_t time,
        bool force_update)
{
    if(!c.startDate)
        return;
    if(GetSymbolicDate(m_groupPlayer, c.startDate) != INT64_MAX)
        if(!force_update)
            return; // this branch is already set.

    SetSymbolicDate(m_groupPlayer, c.startDate, time);
    c.defaultStartDate = time;
    const Scenario::ConstraintDurations& dur = c.constraint().duration;
    if(!dur.isRigid())
        return;

    auto end_date = time + system().toFrame(dur.defaultDuration()) / dur.executionSpeed();
    SetSymbolicDate(m_groupPlayer, c.stopDate, end_date);
    c.defaultDuration = system().toFrame(dur.defaultDuration());

    const Scenario::TimeNodeModel& end_tn = Scenario::endTimeNode(c.constraint(), process());
    if(end_tn.trigger()->active())
        return;

    auto& end_tn_id = end_tn.id();

    auto it = ossia::find_if(timeNodes_pairs(),
                      [=] (auto& e) { return e.element.id() == end_tn_id; });
    ISCORE_ASSERT(it != timeNodes_pairs().end());
    it->component.onDateFixed(end_date, force_update);
}

void ScenarioComponent::onStopDateFixed(const Constraint& c, audio_frame_t time)
{
    if(!c.stopDate)
        return;
    if(GetSymbolicDate(m_groupPlayer, c.stopDate) != INT64_MAX)
        return; // this branch is already set.

    SetSymbolicDate(m_groupPlayer, c.stopDate, time);

    // We don't have anything more to do (it is only called to end interactive constraints.
}

void ScenarioComponent::onSpeedChanged(const Constraint& c, double speed)
{
    const Scenario::ConstraintDurations& dur = c.constraint().duration;
    if(!dur.isRigid())
        return;

    auto cur_date = GetAudioPlayerDateInFrame(m_groupPlayer);
    auto remaining_samples = (c.defaultDuration * (1. - c.constraint().duration.playPercentage())) / speed;
    auto end_date = cur_date + remaining_samples;
    SetSymbolicDate(m_groupPlayer, c.stopDate, end_date);

    const Scenario::TimeNodeModel& end_tn = Scenario::endTimeNode(c.constraint(), process());
    if(end_tn.trigger()->active())
        return;

    auto& end_tn_id = end_tn.id();

    auto it = ossia::find_if(timeNodes_pairs(),
                      [=] (auto& e) { return e.element.id() == end_tn_id; });
    ISCORE_ASSERT(it != timeNodes_pairs().end());
    it->component.onDateFixed(end_date, true);
}

void ScenarioComponent::onDateFixed(
        const TimeNode& c,
        audio_frame_t time,
        bool force_update)
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
        Scenario::EventModel& ev = process().events.at(ev_id);
        if(!ev.condition().hasChildren())
        {
            auto it = ossia::find_if(events_pairs(), [=] (auto& e) { return e.element.id() == ev_id; });
            ISCORE_ASSERT(it != events_pairs().end());
            it->component.onDateFixed(time, force_update);
        }
    }
}

}
}
