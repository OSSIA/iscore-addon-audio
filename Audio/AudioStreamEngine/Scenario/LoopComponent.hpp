#pragma once
#include <Audio/AudioStreamEngine/Scenario/ConstraintComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/EventComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/SyncComponent.hpp>
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
               const Id<score::Component>& id,
               QObject* parent_obj);

       template<typename Component_T, typename Element>
       Component_T* make(
               const Id<score::Component>& id,
               Element& elt);

       template<typename... Args>
       void removing(Args&&...) {}
};

class LoopComponent final : public HierarchicalBaseScenario<
    LoopComponentBase,
    Loop::ProcessModel,
    Interval,
    Event,
    Sync,
    State>
{
    public:
        using HierarchicalBaseScenario<
        LoopComponentBase,
        Loop::ProcessModel,
        Interval,
        Event,
        Sync,
        State>::HierarchicalBaseScenario;

        optional<AudioGraphVertice> visit(AudioGraph& graph) override;

        void makeStream(const Context& ctx) override;
        void stop() override;

    private:
        std::vector<QMetaObject::Connection> m_connections;

};

using LoopComponentFactory = ProcessComponentFactory_T<LoopComponent>;
}
}
