#pragma once
#include <Engine/Executor/ClockManager/ClockManagerFactory.hpp>
#include <Engine/Executor/ClockManager/DefaultClockManager.hpp>
#include <LibAudioStreamMC++.h>


namespace Audio
{
namespace AudioStreamEngine
{
class DocumentPlugin;
class AudioClock final :
        public Engine::Execution::ClockManager
{
    public:
        AudioClock(const Engine::Execution::Context& ctx);

    private:
        // Clock interface
        void play_impl(
                const TimeValue& t,
                Engine::Execution::BaseScenarioElement&) override;
        void pause_impl(Engine::Execution::BaseScenarioElement&) override;
        void resume_impl(Engine::Execution::BaseScenarioElement&) override;
        void stop_impl(Engine::Execution::BaseScenarioElement&) override;

        Engine::Execution::DefaultClockManager m_default;
        AudioStreamEngine::DocumentPlugin& m_audioPlug;
};

class AudioClockFactory final : public Engine::Execution::ClockManagerFactory
{
        ISCORE_CONCRETE("97545b15-d9f5-4001-91e6-2c1fcd43b295")

        QString prettyName() const override;
        std::unique_ptr<Engine::Execution::ClockManager> make(
            const Engine::Execution::Context& ctx) override;
};
}
}

