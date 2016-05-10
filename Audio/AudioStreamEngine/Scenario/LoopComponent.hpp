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
class LoopComponent final : public ProcessComponent_T<Loop::ProcessModel>
{
       COMPONENT_METADATA(Audio::AudioStreamEngine::ScenarioComponent)

        using system_t = Audio::AudioStreamEngine::DocumentPlugin;
        using hierarchy_t =
           BaseScenarioComponentHierarchyManager<
               LoopComponent,
               system_t,
               Loop::ProcessModel,
               ConstraintComponent,
               EventComponent,
               TimeNodeComponent,
               StateComponent
        >;

    public:
       LoopComponent(
               const Id<Component>& id,
               Loop::ProcessModel& scenario,
               const system_t& doc,
               const iscore::DocumentContext& ctx,
               QObject* parent_obj);

       const auto& constraints() const
       { return m_hm.constraints(); }

       void makeStream(const Context& ctx) override;



       template<typename Component_T, typename Element>
       Component_T* make(
               const Id<Component>& id,
               Element& elt,
               const system_t& doc,
               const iscore::DocumentContext& ctx,
               QObject* parent);

        void removing(
                const Scenario::ConstraintModel& elt,
                const ConstraintComponent& comp);

        void removing(
                const Scenario::EventModel& elt,
                const EventComponent& comp);

        void removing(
                const Scenario::TimeNodeModel& elt,
                const TimeNodeComponent& comp);

        void removing(
                const Scenario::StateModel& elt,
                const StateComponent& comp);

    private:
        hierarchy_t m_hm;


        std::map<Id<Scenario::ConstraintModel>, std::pair<SymbolicDate, QMetaObject::Connection>> m_synchros;
        std::map<Id<Scenario::ConstraintModel>, AudioStream> m_csts;

        AudioRendererPtr m_renderer;
        AudioStream m_player;
};

}
}