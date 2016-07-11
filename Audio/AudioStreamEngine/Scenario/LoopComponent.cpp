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
        const Id<iscore::Component>& id,
        QObject* parent_obj):
    ProcessComponent_T{scenario, doc, id, "LoopComponent", parent_obj}
{
}

AudioGraphVertice LoopComponent::visit(AudioGraph& graph)
{
    auto res = boost::add_vertex(this, graph);

    for(auto& constraint : constraints())
    {
        if(auto cst_vtx = constraint.component.visit(graph))
        {
            boost::add_edge(cst_vtx, res, graph);
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

    auto& pattern_cst = *constraints().begin();
    auto sound = pattern_cst.component.getStream();
    if(!sound)
        return;


    // If we are in the "good" case of a non-interactive loop, we
    // can have sample-accurate looping : each loop iteration will be exactly one
    // sample after the following.
    auto& start_node = *timeNodes().begin();
    auto& start_event = *events().begin();
    auto& end_node = *++timeNodes().begin();

    if(!start_node.element.trigger()->active() &&
            !end_node.element.trigger()->active() &&
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

        auto groupPlayer = MakeGroupPlayer();
        auto groupStream = MakeGroupStream(groupPlayer);

        auto start_date = GenSymbolicDate(groupPlayer);
        auto start_con = con(pattern_cst.element, &Scenario::ConstraintModel::executionStarted,
                             this, [=] () {
            SetSymbolicDate(groupPlayer, start_date, GetAudioPlayerDateInFrame(groupPlayer));
        }, Qt::QueuedConnection);
        m_connections.push_back(start_con);

        auto stop_date = GenSymbolicDate(groupPlayer);
        auto stop_con = con(pattern_cst.element, &Scenario::ConstraintModel::executionStopped,
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

template<>
Constraint* LoopComponentBase::make<Constraint, Scenario::ConstraintModel>(
        const Id<iscore::Component>& id,
        Scenario::ConstraintModel& elt)
{
    return new Constraint{elt, system(), id, this};
}

template<>
Event* LoopComponentBase::make<Event, Scenario::EventModel>(
        const Id<iscore::Component>& id,
        Scenario::EventModel& elt)
{
    return new Event{id, elt, system(), this};
}

template<>
TimeNode* LoopComponentBase::make<TimeNode, Scenario::TimeNodeModel>(
        const Id<iscore::Component>& id,
        Scenario::TimeNodeModel& elt)
{
    return new TimeNode{id, elt, system(), this};
}

template<>
State* LoopComponentBase::make<State, Scenario::StateModel>(
        const Id<iscore::Component>& id,
        Scenario::StateModel& elt)
{
    return nullptr;
}

}
}
