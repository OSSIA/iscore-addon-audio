#pragma once
#include <Audio/AudioStreamEngine/Scenario/ConstraintComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/EventComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/TimeNodeComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/StateComponent.hpp>
#include <Scenario/Document/Components/ScenarioComponent.hpp>
#include <QMetaObject>
namespace Audio
{
namespace AudioStreamEngine
{
class ScenarioComponentBase :
        public ProcessComponent_T<Scenario::ProcessModel, false, true>
{
        COMPONENT_METADATA("33a11e0f-d320-4fc7-98e4-714845f21dc8")

    public:
       ScenarioComponentBase(
               Scenario::ProcessModel& scenario,
               DocumentPlugin& doc,
                const Id<iscore::Component>& id,
               QObject* parent_obj);

       template<typename Component_T, typename Element>
       Component_T* make(
               const Id<iscore::Component>& id,
               Element& elt);

       template<typename... Args>
       void removing(Args&&...) { }
};


class ScenarioComponent final : public HierarchicalScenarioComponent<
    ScenarioComponentBase,
    Scenario::ProcessModel,
    Constraint,
    Event,
    TimeNode,
    State>
{
    public:
        using HierarchicalScenarioComponent<
        ScenarioComponentBase,
        Scenario::ProcessModel,
        Constraint,
        Event,
        TimeNode,
        State>::HierarchicalScenarioComponent;
        ~ScenarioComponent();

        optional<AudioGraphVertice> visit(AudioGraph& graph) override;

        void makeStream(const Context& ctx) override;

    private:
        void onDateFixed(const TimeNode& t, audio_frame_t time, bool force_update);
        void onDateFixed(const Event& t, audio_frame_t time, bool force_update);
        void onStartDateFixed(Constraint& t, audio_frame_t time, bool force_update);
        void onStopDateFixed(const Constraint& t, audio_frame_t time);
        void onSpeedChanged(const Constraint& t, double speed);

        AudioRendererPtr m_groupPlayer;
        std::vector<QMetaObject::Connection> m_connections;

};

using ScenarioComponentFactory = ProcessComponentFactory_T<ScenarioComponent>;
}
}
