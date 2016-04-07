#pragma once
#include <Audio/AudioStreamEngine/Scenario/ProcessComponent.hpp>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>
#include <Scenario/Document/Components/ConstraintComponent.hpp>

namespace Audio
{
namespace AudioStreamEngine
{
// The Audio constraint components creates Constraint audio streams
// and contains mixing data (i.e. for each "data" & each "effect" process,
// how much of each data processes goes into effect proccesses.)
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

        const Key& key() const override;

        ConstraintComponent(
                const Id<Component>& id,
                Scenario::ConstraintModel& constraint,
                const system_t& doc,
                const iscore::DocumentContext& ctx,
                QObject* parent_comp);
        ~ConstraintComponent();

        AudioStream makeStream(
                const Context& player,
                SymbolicDate start,
                SymbolicDate end);

        ProcessComponent* make_processComponent(
                const Id<Component> & id,
                ProcessComponentFactory& factory,
                Process::ProcessModel &process,
                const DocumentPlugin &system,
                const iscore::DocumentContext &ctx,
                QObject *parent_component);

        void removing(const Process::ProcessModel& cst, const ProcessComponent& comp);

    private:
        parent_t m_baseComponent;
        std::vector<StreamPair> inputStreams;
        std::vector<StreamPair> fxStreams;
};



}
}
