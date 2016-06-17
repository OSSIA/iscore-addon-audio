#include "AudioClock.hpp"
#include <OSSIA/Executor/ExecutorContext.hpp>
#include <OSSIA/Executor/DocumentPlugin.hpp>
#include <OSSIA/Executor/BaseScenarioElement.hpp>
#include <OSSIA/Executor/ConstraintElement.hpp>
#include <OSSIA/Executor/Settings/ExecutorModel.hpp>

#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>

namespace Audio
{
namespace AudioStreamEngine
{

AudioClock::AudioClock(
        const RecreateOnPlay::Context& ctx):
    ClockManager{ctx},
    m_default{ctx}
{
}

void AudioClock::play_impl(
        const TimeValue& t,
        RecreateOnPlay::BaseScenarioElement& bs)
{
    auto& plug = context.doc.plugin<AudioStreamEngine::DocumentPlugin>();
    plug.play();
    m_default.play(t);
}

void AudioClock::pause_impl(
        RecreateOnPlay::BaseScenarioElement& bs)
{
    ISCORE_TODO;
    m_default.pause();
}

void AudioClock::resume_impl(
        RecreateOnPlay::BaseScenarioElement& bs)
{
    ISCORE_TODO;
    m_default.resume();
}

void AudioClock::stop_impl(
        RecreateOnPlay::BaseScenarioElement& bs)
{
    auto& plug = context.doc.plugin<AudioStreamEngine::DocumentPlugin>();
    plug.stop();
    m_default.stop();
}

std::unique_ptr<RecreateOnPlay::ClockManager> AudioClockFactory::make(
        const RecreateOnPlay::Context& ctx)
{
    return std::make_unique<AudioClock>(ctx);
}

}
}
