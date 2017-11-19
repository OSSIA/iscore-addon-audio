#include "AudioClock.hpp"
#include <Engine/Executor/ExecutorContext.hpp>
#include <Engine/Executor/DocumentPlugin.hpp>
#include <Engine/Executor/BaseScenarioComponent.hpp>
#include <Engine/Executor/IntervalComponent.hpp>
#include <Engine/Executor/Settings/ExecutorModel.hpp>

#include <Audio/Settings/Card/CardSettingsModel.hpp>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>
#include <Engine/OSSIA2score.hpp>
#include <Engine/score2OSSIA.hpp>

#include <Audio/AudioStreamEngine/Streams/AudioStreamIScoreExtensions.h>


namespace Audio
{
namespace AudioStreamEngine
{

AudioClock::AudioClock(
        const Engine::Execution::Context& ctx):
    ClockManager{ctx},
    m_default{ctx},
    m_audioPlug{context.doc.plugin<AudioStreamEngine::DocumentPlugin>()}
{
    auto& bs = context.scenario;
    if(!bs.active())
        return;
}

void AudioClock::play_impl(
        const TimeVal& t,
        Engine::Execution::BaseScenarioElement& bs)
{
    auto stream = m_audioPlug.makeStream(bs.baseInterval().scoreInterval());
    if(!stream)
    {
        qDebug("No stream!");
        return;
    }

    m_default.play(t);
    AudioStream finals = MakeIScoreExecutor(stream, *bs.baseInterval().OSSIAInterval());

    auto& player = m_audioPlug.audioContext.audio.player;
    StartSound(player, finals, GenRealDate(player, 0));
    auto rate = context.doc.app.settings<Audio::Settings::Model>().getRate();
    m_audioPlug.offsetting = true;
    SetPosAudioPlayer(player, std::llround(rate * t.msec() / 1000.));
    m_audioPlug.offsetting = false;
    StartAudioPlayer(player);
}

void AudioClock::pause_impl(
        Engine::Execution::BaseScenarioElement& bs)
{
    m_default.pause();
    PauseAudioPlayer(m_audioPlug.audioContext.audio.player);
}

void AudioClock::resume_impl(
        Engine::Execution::BaseScenarioElement& bs)
{
    m_default.resume();
    ContAudioPlayer(m_audioPlug.audioContext.audio.player);
}

void AudioClock::stop_impl(
        Engine::Execution::BaseScenarioElement& bs)
{
    m_audioPlug.stop();
    m_default.stop();
}

std::unique_ptr<Engine::Execution::ClockManager> AudioClockFactory::make(
        const Engine::Execution::Context& ctx)
{
    return std::make_unique<AudioClock>(ctx);
}

QString AudioClockFactory::prettyName() const
{
    return QObject::tr("Audio");
}

std::function<ossia::time_value (const TimeVal&)>
AudioClockFactory::makeTimeFunction(const score::DocumentContext& ctx) const
{
  auto rate = ctx.app.settings<Audio::Settings::Model>().getRate();
  return [=] (const TimeVal& v) -> ossia::time_value {
    // Go from milliseconds to samples
    // 1000 ms = sr samples
    // x ms    = k samples
    return v.isInfinite()
        ? ossia::Infinite
        : ossia::time_value(std::llround(rate * v.msec() / 1000.));
  };
}

std::function<TimeVal(const ossia::time_value&)>
AudioClockFactory::makeReverseTimeFunction(const score::DocumentContext& ctx) const
{
  auto rate = ctx.app.settings<Audio::Settings::Model>().getRate();
  return [=] (const ossia::time_value& v) -> TimeVal {
    return v.infinite()
        ? TimeVal{PositiveInfinity{}}
        : TimeVal::fromMsecs(1000. * v.impl / rate);
  };
}


}
}
