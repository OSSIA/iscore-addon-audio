#include "AudioClock.hpp"
#include <OSSIA/Executor/ExecutorContext.hpp>
#include <OSSIA/Executor/DocumentPlugin.hpp>
#include <OSSIA/Executor/BaseScenarioElement.hpp>
#include <OSSIA/Executor/ConstraintElement.hpp>
#include <OSSIA/Executor/Settings/ExecutorModel.hpp>

#include <Audio/Settings/Card/CardSettingsModel.hpp>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>

#include <Audio/AudioStreamEngine/GroupAudioStream.h>


namespace Audio
{
namespace AudioStreamEngine
{

AudioClock::AudioClock(
        const RecreateOnPlay::Context& ctx):
    ClockManager{ctx},
    m_default{ctx},
    m_audioPlug{context.doc.plugin<AudioStreamEngine::DocumentPlugin>()}
{
    auto bs = context.sys.baseScenario();
    if(!bs)
        return;

    auto& audio = context.doc.app.settings<Audio::Settings::Model>();

    OSSIA::TimeConstraint& ossia_cst = *bs->baseConstraint()->OSSIAConstraint();

    ossia_cst.setDriveMode(OSSIA::Clock::DriveMode::EXTERNAL);
    // Number of milliseconds in each step -> we tick once per buffer
    ossia_cst.setGranularity(1000 * audio.getBufferSize() / audio.getRate() );
}

void AudioClock::play_impl(
        const TimeValue& t,
        RecreateOnPlay::BaseScenarioElement& bs)
{
    auto stream = m_audioPlug.makeStream();
    if(!stream)
    {
        qDebug("No stream!");
        return;
    }

    m_default.play(t);
    AudioStream finals = MakeIScoreExecutor(stream, *bs.baseConstraint()->OSSIAConstraint());
    auto& player = m_audioPlug.context.audio.player;


    StartSound(player, finals, GenRealDate(player, 0));
    StartAudioPlayer(player);
}

void AudioClock::pause_impl(
        RecreateOnPlay::BaseScenarioElement& bs)
{
    m_default.pause();
    PauseAudioPlayer(m_audioPlug.context.audio.player);
}

void AudioClock::resume_impl(
        RecreateOnPlay::BaseScenarioElement& bs)
{
    m_default.resume();
    ContAudioPlayer(m_audioPlug.context.audio.player);
}

void AudioClock::stop_impl(
        RecreateOnPlay::BaseScenarioElement& bs)
{
    m_audioPlug.stop();
    m_default.stop();
}

std::unique_ptr<RecreateOnPlay::ClockManager> AudioClockFactory::make(
        const RecreateOnPlay::Context& ctx)
{
    return std::make_unique<AudioClock>(ctx);
}

QString AudioClockFactory::prettyName() const
{
    return QObject::tr("Audio");
}


}
}
