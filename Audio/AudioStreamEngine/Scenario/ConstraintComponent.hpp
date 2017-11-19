#pragma once
#include <Audio/AudioStreamEngine/Scenario/ProcessComponent.hpp>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>
#include <Audio/AudioStreamEngine/AudioComponent.hpp>
#include <Scenario/Document/Components/IntervalComponent.hpp>
#include <score/model/ComponentHierarchy.hpp>
#include <functional>

namespace Audio
{
namespace Mix
{
class ProcessModel;
}
namespace AudioStreamEngine
{
class IntervalBase :
        public Scenario::IntervalComponent<Component>
{
        COMMON_COMPONENT_METADATA("13521db6-0de7-462c-9a43-57612a250216")
    public:
        static const constexpr bool is_unique = true;
        using StreamPair = std::pair<Process::ProcessModel&, AudioStream>;
        using model_t = Process::ProcessModel;
        using system_t = Audio::AudioStreamEngine::DocumentPlugin;
        using component_t = Audio::AudioStreamEngine::ProcessComponent;
        using component_factory_t = Audio::AudioStreamEngine::ProcessComponentFactory;
        using component_factory_list_t = Audio::AudioStreamEngine::ProcessComponentFactoryList;

        IntervalBase(
                Scenario::IntervalModel& interval,
                IntervalBase::system_t& doc,
                const Id<score::Component>& id,
                QObject* parent_comp);
        ~IntervalBase();

        ProcessComponent* make(
                const Id<score::Component> & id,
                ProcessComponentFactory& factory,
                Process::ProcessModel &process);

        bool removing(const Process::ProcessModel& cst, const ProcessComponent& comp);
        template <typename... Args>
        void removed(Args&&...)
        {
        }
};

class Interval final :
        public score::PolymorphicComponentHierarchy<IntervalBase>
{
        using parent_t = score::PolymorphicComponentHierarchy<IntervalBase>;
    public:
        Interval(
                Scenario::IntervalModel& interval,
                IntervalBase::system_t& doc,
                const Id<score::Component>& id,
                QObject* parent_comp);

        ~Interval();
        optional<AudioGraphVertice> visit(AudioGraph& graph) override;
        void makeStream(const Context& player) override;
        AudioStream getStream() const { return m_stream; }

        AudioStream makeInputMix(const Id<Process::ProcessModel>& target);
        Mix::ProcessModel* findMix() const;

        void stop();

        std::function<void(audio_frame_t, bool)> onStartDateFixed;
        std::function<void(audio_frame_t)> onStopDateFixed;

        SymbolicDate date;
        SymbolicDate stopDate;

        audio_frame_t defaultDuration = INT64_MAX;

    private:
        double m_shift{1.0};
        double m_stretch{1.0};
};

}
}
