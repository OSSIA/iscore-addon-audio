#pragma once
#include <Audio/AudioStreamEngine/Scenario/ConstraintComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/EventComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/SyncComponent.hpp>
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
                const Id<score::Component>& id,
               QObject* parent_obj);

       template<typename Component_T, typename Element>
       Component_T* make(
               const Id<score::Component>& id,
               Element& elt);

       template<typename... Args>
       bool removing(Args&&...) { return true; }
       template<typename... Args>
       void removed(Args&&...) { }
};


class ScenarioComponent final : public HierarchicalScenarioComponent<
    ScenarioComponentBase,
    Scenario::ProcessModel,
    Interval,
    Event,
    Sync,
    State>
{
    public:
        using HierarchicalScenarioComponent<
        ScenarioComponentBase,
        Scenario::ProcessModel,
        Interval,
        Event,
        Sync,
        State>::HierarchicalScenarioComponent;
        ~ScenarioComponent();

        optional<AudioGraphVertice> visit(AudioGraph& graph) override;

        void makeStream(const Context& ctx) override;

        void stop() override;
    private:
        void onDateFixed(const Sync& t, audio_frame_t time, bool force_update);
        void onDateFixed(const Event& t, audio_frame_t time, bool force_update);
        void onStartDateFixed(Interval& t, audio_frame_t time, bool force_update);
        void onStopDateFixed(const Interval& t, audio_frame_t time);
        void onSpeedChanged(const Interval& t, double speed);

        AudioRendererPtr m_groupPlayer;
        std::vector<QMetaObject::Connection> m_connections;

};

using ScenarioComponentFactory = ProcessComponentFactory_T<ScenarioComponent>;
}
}
