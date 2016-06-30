#include "LoopComponent.hpp"
#include <Scenario/Process/Algorithms/Accessors.hpp>
#include <Audio/AudioStreamEngine/Utility.hpp>
#include <Audio/AudioStreamEngine/Streams/AudioStreamIScoreExtensions.h>
namespace Audio
{
namespace AudioStreamEngine
{

LoopComponent::LoopComponent(
        const Id<iscore::Component>& id,
        Loop::ProcessModel& scenario,
        LoopComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent_obj):
    ProcessComponent_T{scenario, id, "LoopComponent", parent_obj},
    m_hm{*this, scenario, doc, ctx, this}
{
}

void LoopComponent::makeStream(const Context& ctx)
{
    for(auto& elt : m_synchros)
    {
        QObject::disconnect(elt.second.second);
    }
    m_synchros.clear();
    m_csts.clear();

    auto& pattern_cst = m_hm.constraints().at(0);
    auto sound = pattern_cst.component.getStream();
    if(!sound)
        return;


    // If we are in the "good" case of a non-interactive loop, we
    // can have sample-accurate looping : each loop iteration will be exactly one
    // sample after the following.
    auto& start_node = m_hm.timeNodes().at(0);
    auto& start_event = m_hm.events().at(0);
    auto& end_node = m_hm.timeNodes().at(1);

    if(!start_node.element.trigger()->active() &&
       !end_node.element.trigger()->active() &&
       !start_event.element.condition().hasChildren())
    {
        m_stream = MakeSend(MakeFixedLoopSound(sound, toFrame(pattern_cst.element.duration.defaultDuration())));
        return;
    }
    else
    {
        // For now "full interactive mode"
        // Then we have to handle the :
        // - interactive at both start and end case
        // - interactive at only start
        // - interactive at only end

        m_groupPlayer = MakeGroupPlayer();
        m_groupStream = MakeGroupStream(m_groupPlayer);

        auto start_date = GenSymbolicDate(m_groupPlayer);
        auto start_con = con(pattern_cst.element, &Scenario::ConstraintModel::executionStarted,
                             this, [=] () {
            SetSymbolicDate(m_groupPlayer, start_date, GetAudioPlayerDateInFrame(m_groupPlayer));
        }, Qt::QueuedConnection);
        m_synchros.insert(std::make_pair(pattern_cst.element.id(), std::make_pair(start_date, start_con)));

        auto stop_date = GenSymbolicDate(m_groupPlayer);
        auto stop_con = con(pattern_cst.element, &Scenario::ConstraintModel::executionStopped,
                            this, [=] () {
            SetSymbolicDate(m_groupPlayer, stop_date, GetAudioPlayerDateInFrame(m_groupPlayer));
            ResetSound(m_groupStream);
        }, Qt::QueuedConnection);
        m_synchros.insert(std::make_pair(pattern_cst.element.id(), std::make_pair(stop_date, stop_con)));

        m_csts.insert(
                    std::make_pair(
                        pattern_cst.element.id(),
                        sound
                        )
                    );
        StartSound(m_groupPlayer, sound, start_date);
        StopSound(m_groupPlayer, sound, stop_date);


    m_stream = MakeSend(m_groupStream);

    }
}

template<>
ConstraintComponent* LoopComponent::make<ConstraintComponent, Scenario::ConstraintModel>(
        const Id<iscore::Component>& id,
        Scenario::ConstraintModel& elt,
        LoopComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent)
{
    auto comp = new ConstraintComponent{id, elt, doc, ctx, parent};
    comp->onStartDateFixed = [=] (audio_frame_t t, bool) { onStartDateFixed(*comp, t); };
    comp->onStopDateFixed = [=] (audio_frame_t t) { onStopDateFixed(*comp, t); };
    return comp;
}

template<>
EventComponent* LoopComponent::make<EventComponent, Scenario::EventModel>(
        const Id<iscore::Component>& id,
        Scenario::EventModel& elt,
        LoopComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent)
{
    auto comp = new EventComponent{id, elt, doc, ctx, parent};
    comp->onDateFixed = [=] (audio_frame_t t, bool) { onDateFixed(*comp, t); };
    return comp;
}

template<>
TimeNodeComponent* LoopComponent::make<TimeNodeComponent, Scenario::TimeNodeModel>(
        const Id<iscore::Component>& id,
        Scenario::TimeNodeModel& elt,
        LoopComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent)
{
    auto comp = new TimeNodeComponent{id, elt, doc, ctx, parent};
    comp->onDateFixed = [=] (audio_frame_t t, bool) { onDateFixed(*comp, t); };
    return comp;
}

template<>
StateComponent* LoopComponent::make<StateComponent, Scenario::StateModel>(
        const Id<iscore::Component>& id,
        Scenario::StateModel& elt,
        LoopComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent)
{
    return new StateComponent{id, elt, doc, ctx, parent};
}

void LoopComponent::removing(const Scenario::ConstraintModel& elt, const ConstraintComponent& comp)
{
}

void LoopComponent::removing(const Scenario::EventModel& elt, const EventComponent& comp)
{
}

void LoopComponent::removing(const Scenario::TimeNodeModel& elt, const TimeNodeComponent& comp)
{
}

void LoopComponent::removing(const Scenario::StateModel& elt, const StateComponent& comp)
{
}

void LoopComponent::onDateFixed(const EventComponent& c, audio_frame_t time)
{
    /*
    auto& states = c.event.states();
    for(auto& st_id : states)
    {
        auto& st = m_hm.scenario.states.at(st_id);
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
    */
}

void LoopComponent::onStartDateFixed(const ConstraintComponent& c, audio_frame_t time)
{
    /*
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
*/
}

void LoopComponent::onStopDateFixed(const ConstraintComponent& c, audio_frame_t time)
{
    /*
    if(!c.stopDate)
        return;
    if(GetSymbolicDate(m_groupPlayer, c.stopDate) != INT64_MAX)
        return; // this branch is already set.

    SetSymbolicDate(m_groupPlayer, c.stopDate, time);
    ResetSound(m_groupStream);

    // We don't have anything more to do (it is only called to end interactive constraints.
    */
}

audio_frame_t LoopComponent::toFrame(const TimeValue& t) const
{
    return t.msec() * m_hm.system.context.audio.sample_rate / 1000.0;
}

void LoopComponent::onDateFixed(const TimeNodeComponent& c, audio_frame_t time)
{
    /*
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
    */
}
}
}
