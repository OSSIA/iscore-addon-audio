#pragma once
#include <Audio/AudioStreamEngine/Scenario/ProcessComponent.hpp>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>
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
class ConstraintComponent final :
        public iscore::Component
{
    public:
        using StreamPair = std::pair<Process::ProcessModel&, AudioStream>;
        using system_t = Audio::AudioStreamEngine::DocumentPlugin;
        using process_component_t = Audio::AudioStreamEngine::ProcessComponent;
        using process_component_factory_t = Audio::AudioStreamEngine::ProcessComponentFactory;
        using process_component_factory_list_t = Audio::AudioStreamEngine::ProcessComponentFactoryList;

        using parent_t = ::ConstraintComponentHierarchyManager<
            ConstraintComponent,
            system_t,
            process_component_t,
            process_component_factory_list_t
        >;

        const Scenario::ConstraintModel& constraint() const
        { return m_hm.constraint; }

        const Key& key() const override;

        ConstraintComponent(
                const Id<Component>& id,
                Scenario::ConstraintModel& constraint,
                system_t& doc,
                const iscore::DocumentContext& ctx,
                QObject* parent_comp);
        ~ConstraintComponent();

        void makeStream(const Context& player);
        AudioStream getStream() const { return m_stream; }

        AudioStream makeInputMix(const Id<Process::ProcessModel>& target);

        ProcessComponent* make_processComponent(
                const Id<Component> & id,
                ProcessComponentFactory& factory,
                Process::ProcessModel &process,
                DocumentPlugin &system,
                const iscore::DocumentContext &ctx,
                QObject *parent_component);

        void removing(const Process::ProcessModel& cst, const ProcessComponent& comp);

        const auto& processes() const
        { return m_hm.processes(); }

        Mix::ProcessModel* findMix() const;

        std::function<void(audio_frame_t, bool)> onStartDateFixed;
        std::function<void(audio_frame_t)> onStopDateFixed;

        SymbolicDate startDate;
        SymbolicDate stopDate;

        audio_frame_t defaultStartDate = INT64_MAX;
        audio_frame_t defaultDuration = INT64_MAX;
    private:

        audio_frame_t toFrame(TimeValue t) const;

        parent_t m_hm;
        double m_shift{1.0};
        double m_stretch{1.0};

        AudioStream m_stream{};
};



}
}
