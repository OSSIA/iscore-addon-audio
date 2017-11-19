#include "LoopComponent.hpp"
#include <Scenario/Process/Algorithms/Accessors.hpp>
#include <Audio/AudioStreamEngine/Utility.hpp>
#include <Audio/AudioStreamEngine/Streams/AudioStreamIScoreExtensions.h>
namespace Audio
{
namespace AudioStreamEngine
{

LoopComponentBase::LoopComponentBase(
        Loop::ProcessModel& scenario,
        DocumentPlugin& doc,
        const Id<score::Component>& id,
        QObject* parent_obj):
    ProcessComponent_T{scenario, doc, id, "LoopComponent", parent_obj}
{
}

optional<AudioGraphVertice> LoopComponent::visit(AudioGraph& graph)
{
    auto res = boost::add_vertex(this, graph);

    for(auto& interval : intervals())
    {
        if(auto cst_vtx = interval.component.visit(graph))
        {
            boost::add_edge(*cst_vtx, res, graph);
        }
    }

    return res;
}

void LoopComponent::makeStream(const Context& ctx)
{
    for(auto& elt : m_connections)
    {
        QObject::disconnect(elt);
    }
    m_connections.clear();

    auto& pattern_cst = *intervals().begin();
    auto sound = pattern_cst.component.getStream();
    if(!sound)
        return;


    // If we are in the "good" case of a non-interactive loop, we
    // can have sample-accurate looping : each loop iteration will be exactly one
    // sample after the following.
    auto& start_node = *timeSyncs().begin();
    auto& start_event = *events().begin();
    auto& end_node = *++timeSyncs().begin();

    if(!start_node.element.active() &&
            !end_node.element.active() &&
            !start_event.element.condition().hasChildren())
    {
        m_stream = MakeSend(MakeFixedLoopSound(sound, system().toFrame(pattern_cst.element.duration.defaultDuration())));
        return;
    }
    else
    {
        // For now "full interactive mode"
        // Then we have to handle the :
        // - interactive at both start and end case
        // - interactive at only start
        // - interactive at only end

        auto groupPlayer = MakeGroupPlayer(ctx.audio.renderer_info.fOutput);
        auto groupStream = MakeGroupStream(groupPlayer);

        auto start_date = GenSymbolicDate(groupPlayer);
        auto start_con = con(pattern_cst.element, &Scenario::IntervalModel::executionStarted,
                             this, [=] () {
            SetSymbolicDate(groupPlayer, start_date, GetAudioPlayerDateInFrame(groupPlayer));
        }, Qt::QueuedConnection);
        m_connections.push_back(start_con);

        auto stop_date = GenSymbolicDate(groupPlayer);
        auto stop_con = con(pattern_cst.element, &Scenario::IntervalModel::executionStopped,
                            this, [=] () {
            SetSymbolicDate(groupPlayer, stop_date, GetAudioPlayerDateInFrame(groupPlayer));
            ResetSound(groupStream);
        }, Qt::QueuedConnection);
        m_connections.push_back(stop_con);

        StartSound(groupPlayer, sound, start_date);
        StopSound(groupPlayer, sound, stop_date);


        m_stream = MakeSend(groupStream);

    }
}

void LoopComponent::stop()
{
  for(auto& interval : intervals())
    interval.component.stop();
}

template<>
Interval* LoopComponentBase::make<Interval, Scenario::IntervalModel>(
        const Id<score::Component>& id,
        Scenario::IntervalModel& elt)
{
    return new Interval{elt, system(), id, this};
}

template<>
Event* LoopComponentBase::make<Event, Scenario::EventModel>(
        const Id<score::Component>& id,
        Scenario::EventModel& elt)
{
    return new Event{id, elt, system(), this};
}

template<>
Sync* LoopComponentBase::make<Sync, Scenario::TimeSyncModel>(
        const Id<score::Component>& id,
        Scenario::TimeSyncModel& elt)
{
    return new Sync{id, elt, system(), this};
}

template<>
State* LoopComponentBase::make<State, Scenario::StateModel>(
        const Id<score::Component>& id,
        Scenario::StateModel& elt)
{
    // TODO if the state is missing, an assert is triggered in HierarchicalBaseInterval's
    // setup. Fix this to do nothing if there is no component
    return new State{id, elt, system(), this};
}

}
}
