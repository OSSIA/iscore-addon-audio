#pragma once
#include <Audio/AudioStreamEngine/Scenario/ConstraintComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/EventComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/TimeNodeComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/StateComponent.hpp>
#include <Scenario/Document/Components/ScenarioComponent.hpp>
#include <Loop/LoopProcessModel.hpp>
#include <QMetaObject>
namespace Audio
{
namespace AudioStreamEngine
{
class LoopComponentBase :
        public ProcessComponent_T<Loop::ProcessModel, false, true>
{
        COMPONENT_METADATA("ab3dd763-0748-4ed1-bd07-2be6a17d52b1")

    public:
       LoopComponentBase(
               Loop::ProcessModel& scenario,
               DocumentPlugin& doc,
               const Id<Component>& id,
               QObject* parent_obj);

       template<typename Component_T, typename Element>
       Component_T* make(
               const Id<Component>& id,
               Element& elt);

       template<typename... Args>
       void removing(Args&&...) {}

        audio_frame_t toFrame(const TimeValue& t) const;

};

class LoopComponent final : public HierarchicalBaseScenario<
    LoopComponentBase,
    Loop::ProcessModel,
    Constraint,
    Event,
    TimeNode,
    State>
{
    public:
        using HierarchicalBaseScenario<
        LoopComponentBase,
        Loop::ProcessModel,
        Constraint,
        Event,
        TimeNode,
        State>::HierarchicalBaseScenario;

        void makeStream(const Context& ctx) override;

    private:
        std::vector<QMetaObject::Connection> m_connections;

};

AUDIO_PROCESS_COMPONENT_FACTORY(LoopComponentFactory, "1dee91f9-3eb9-4e51-93a3-7ee696c2f357", LoopComponent, Loop::ProcessModel)
}
}
