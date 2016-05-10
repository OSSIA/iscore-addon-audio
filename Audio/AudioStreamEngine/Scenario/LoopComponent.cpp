#include "LoopComponent.hpp"
#include <Scenario/Process/Algorithms/Accessors.hpp>
#include <Audio/AudioStreamEngine/Utility.hpp>
#include <Audio/AudioStreamEngine/GroupAudioStream.h>
namespace Audio
{
namespace AudioStreamEngine
{

LoopComponent::LoopComponent(
        const Id<iscore::Component>& id,
        Loop::ProcessModel& scenario,
        const LoopComponent::system_t& doc,
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

    m_renderer = MakeGroupPlayer();
    m_player = MakeGroupStream(m_renderer);

    for(const hierarchy_t::ConstraintPair& cst : m_hm.constraints())
    {
        auto sound = cst.component.getStream();
        if(!sound)
            continue;

        auto start_date = GenSymbolicDate(m_renderer);
        auto start_con = con(cst.element, &Scenario::ConstraintModel::executionStarted,
                           this, [=] () {
            SetSymbolicDate(m_renderer, start_date, GetAudioPlayerDateInFrame(m_renderer));
        }, Qt::QueuedConnection);
        m_synchros.insert(std::make_pair(cst.element.id(), std::make_pair(start_date, start_con)));

        auto stop_date = GenSymbolicDate(m_renderer);
        auto stop_con = con(cst.element, &Scenario::ConstraintModel::executionStopped,
                                 this, [=] () {
            SetSymbolicDate(m_renderer, stop_date, GetAudioPlayerDateInFrame(m_renderer));
            ResetSound(m_player);
        }, Qt::QueuedConnection);
        m_synchros.insert(std::make_pair(cst.element.id(), std::make_pair(stop_date, stop_con)));

        m_csts.insert(
                    std::make_pair(
                        cst.element.id(),
                        sound
                        )
                    );
        qDebug() << "Starting" << cst.element.metadata.name();

        StartSound(m_renderer, sound, start_date);
        StopSound(m_renderer, sound, stop_date);
    }

    m_stream = MakeSend(m_player);
}

template<>
ConstraintComponent* LoopComponent::make<ConstraintComponent, Scenario::ConstraintModel>(
        const Id<iscore::Component>& id,
        Scenario::ConstraintModel& elt,
        const LoopComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent)
{
    return new ConstraintComponent{id, elt, doc, ctx, parent};
}

template<>
EventComponent* LoopComponent::make<EventComponent, Scenario::EventModel>(
        const Id<iscore::Component>& id,
        Scenario::EventModel& elt,
        const LoopComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent)
{
    return new EventComponent{id, elt, doc, ctx, parent};
}

template<>
TimeNodeComponent* LoopComponent::make<TimeNodeComponent, Scenario::TimeNodeModel>(
        const Id<iscore::Component>& id,
        Scenario::TimeNodeModel& elt,
        const LoopComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent)
{
    return new TimeNodeComponent{id, elt, doc, ctx, parent};
}

template<>
StateComponent* LoopComponent::make<StateComponent, Scenario::StateModel>(
        const Id<iscore::Component>& id,
        Scenario::StateModel& elt,
        const LoopComponent::system_t& doc,
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

}
}
