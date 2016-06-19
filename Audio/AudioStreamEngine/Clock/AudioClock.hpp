#pragma once
#include <OSSIA/Executor/ClockManager/ClockManagerFactory.hpp>
#include <OSSIA/Executor/ClockManager/DefaultClockManager.hpp>
#include <LibAudioStreamMC++.h>


namespace Audio
{
namespace AudioStreamEngine
{
class DocumentPlugin;
class AudioClock final :
        public RecreateOnPlay::ClockManager
{
    public:
        AudioClock(const RecreateOnPlay::Context& ctx);

    private:
        // Clock interface
        void play_impl(
                const TimeValue& t,
                RecreateOnPlay::BaseScenarioElement&) override;
        void pause_impl(RecreateOnPlay::BaseScenarioElement&) override;
        void resume_impl(RecreateOnPlay::BaseScenarioElement&) override;
        void stop_impl(RecreateOnPlay::BaseScenarioElement&) override;

        RecreateOnPlay::DefaultClockManager m_default;
        AudioStreamEngine::DocumentPlugin& m_audioPlug;
};

class AudioClockFactory final : public RecreateOnPlay::ClockManagerFactory
{
        ISCORE_CONCRETE_FACTORY_DECL("97545b15-d9f5-4001-91e6-2c1fcd43b295")

        QString prettyName() const override;
        std::unique_ptr<RecreateOnPlay::ClockManager> make(
            const RecreateOnPlay::Context& ctx) override;
};
}
}

