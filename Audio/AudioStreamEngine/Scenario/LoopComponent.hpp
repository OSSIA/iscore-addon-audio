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
class LoopComponent final :
        public ProcessComponent_T<Loop::ProcessModel, false, true>
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
               system_t& doc,
               const iscore::DocumentContext& ctx,
               QObject* parent_obj);

       const auto& constraints() const
       { return m_hm.constraints(); }

       void makeStream(const Context& ctx) override;



       template<typename Component_T, typename Element>
       Component_T* make(
               const Id<Component>& id,
               Element& elt,
               system_t& doc,
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
        void onDateFixed(const TimeNodeComponent& t, audio_frame_t time);
        void onDateFixed(const EventComponent& t, audio_frame_t time);
        void onStartDateFixed(const ConstraintComponent& t, audio_frame_t time);
        void onStopDateFixed(const ConstraintComponent& t, audio_frame_t time);

        audio_frame_t toFrame(const TimeValue& t) const;

        hierarchy_t m_hm;


        std::map<Id<Scenario::ConstraintModel>, std::pair<SymbolicDate, QMetaObject::Connection>> m_synchros;
        std::map<Id<Scenario::ConstraintModel>, AudioStream> m_csts;

        AudioRendererPtr m_groupPlayer;
        AudioStream m_groupStream;
};

AUDIO_COMPONENT_FACTORY(LoopComponentFactory, "1dee91f9-3eb9-4e51-93a3-7ee696c2f357", LoopComponent, Loop::ProcessModel)
}
}
