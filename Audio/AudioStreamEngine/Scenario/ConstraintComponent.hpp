#pragma once
#include <Audio/AudioStreamEngine/Scenario/ProcessComponent.hpp>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>
#include <Audio/AudioStreamEngine/AudioComponent.hpp>
#include <Scenario/Document/Components/ConstraintComponent.hpp>
#include <functional>

namespace Audio
{
namespace Mix
{
class ProcessModel;
}
namespace AudioStreamEngine
{
class ConstraintBase :
        public Scenario::ConstraintComponent<Component>
{
        COMPONENT_METADATA("13521db6-0de7-462c-9a43-57612a250216")
    public:
        static const constexpr bool is_unique = true;
        using StreamPair = std::pair<Process::ProcessModel&, AudioStream>;
        using system_t = Audio::AudioStreamEngine::DocumentPlugin;
        using process_component_t = Audio::AudioStreamEngine::ProcessComponent;
        using process_component_factory_t = Audio::AudioStreamEngine::ProcessComponentFactory;
        using process_component_factory_list_t = Audio::AudioStreamEngine::ProcessComponentFactoryList;

        ConstraintBase(
                Scenario::ConstraintModel& constraint,
                ConstraintBase::system_t& doc,
                const Id<iscore::Component>& id,
                QObject* parent_comp);
        ~ConstraintBase();

        ProcessComponent* make_processComponent(
                const Id<iscore::Component> & id,
                ProcessComponentFactory& factory,
                Process::ProcessModel &process);

        void removing(const Process::ProcessModel& cst, const ProcessComponent& comp);

};

class Constraint final : public ConstraintComponentHierarchyManager<
    ConstraintBase,
    ConstraintBase::process_component_t,
    ConstraintBase::process_component_factory_list_t>
{
        using parent_t = ConstraintComponentHierarchyManager<
        ConstraintBase,
        ConstraintBase::process_component_t,
        ConstraintBase::process_component_factory_list_t>;
    public:
        Constraint(
                Scenario::ConstraintModel& constraint,
                ConstraintBase::system_t& doc,
                const Id<iscore::Component>& id,
                QObject* parent_comp);

        AudioGraphVertice visit(AudioGraph& graph) override;
        void makeStream(const Context& player) override;
        AudioStream getStream() const { return m_stream; }

        AudioStream makeInputMix(const Id<Process::ProcessModel>& target);
        Mix::ProcessModel* findMix() const;

        std::function<void(audio_frame_t, bool)> onStartDateFixed;
        std::function<void(audio_frame_t)> onStopDateFixed;

        SymbolicDate startDate;
        SymbolicDate stopDate;

        audio_frame_t defaultStartDate = INT64_MAX;
        audio_frame_t defaultDuration = INT64_MAX;

    private:
        audio_frame_t toFrame(TimeValue t) const;

        double m_shift{1.0};
        double m_stretch{1.0};
};

}
}
