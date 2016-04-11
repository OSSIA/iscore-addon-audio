#include "AudioDocumentPlugin.hpp"
#include <Scenario/Process/ScenarioModel.hpp>
#include <Loop/LoopProcessModel.hpp>
#include <map>
#include <Scenario/Process/Algorithms/Accessors.hpp>
#include <core/document/DocumentModel.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentModel.hpp>
#include <Audio/AudioStreamEngine/Scenario/ConstraintComponent.hpp>
#include <Audio/AudioStreamEngine/AudioApplicationPlugin.hpp>

namespace Audio
{
namespace AudioStreamEngine
{

AudioStream CreateAudioStream(const Loop::ProcessModel& loop);
AudioStream CreateAudioStream(const Loop::ProcessModel& loop)
{
    // Apply to the constraint
    // If trigger at the end, see:
    /*
    date = GetCurDate();
    symb1 = GenSymbolicDate(gAudioPlayer);

    s1 = MakeLoopSound(MakeRegionSound(FILENAME1, 5*SR, 100*SR), INFINI);
    s2 = MakeRegionSound(FILENAME2, 5*SR, 10*SR);

    StartSound(gAudioPlayer, s1, GenRealDate(gAudioPlayer, date));
    StopSound(gAudioPlayer, s1, symb1);
    StartSound(gAudioPlayer, s2, symb1);

    // Instancie la date symbolique à la date courante "capturée"
    set_symbolic_date(symb1);
    */


    return {};
}

void DocumentPlugin::play()
{
    // First find the root constraint
    auto doc = dynamic_cast<Scenario::ScenarioDocumentModel*>(&m_ctx.doc.document.model().modelDelegate());
    if(!doc)
        return;

    qDebug() << (void*)m_ctx.audio.player;
    if(!m_ctx.audio.player)
    {
        iscore::AppContext().components.applicationPlugin<Audio::AudioStreamEngine::ApplicationPlugin>().startEngine();
    }

    // Create our tree

    // TODO make id from components !!!!
    ConstraintComponent* comp = nullptr;
    auto it = doc->baseConstraint().components.find(Id<iscore::Component>{1});
    if(it != doc->baseConstraint().components.end())
    {
        comp = dynamic_cast<ConstraintComponent*>(&(*it));
    }
    else
    {
        comp = new ConstraintComponent(
                    Id<iscore::Component>{1},
                    doc->baseConstraint(),
                    *this,
                    m_ctx.doc,
                    this);
        doc->baseConstraint().components.add(comp);
    }

    // Play
    StartAudioPlayer(m_ctx.audio.player);

    if(comp)
    {
        m_stream = comp->makeStream(
                    m_ctx,
                    GenRealDate(m_ctx.audio.player, 0),
                    GenSymbolicDate(m_ctx.audio.player));
    }
    else
    {
        qDebug("No component!");
    }

    if(m_stream)
    {
        StartSound(m_ctx.audio.player, m_stream, GenRealDate(m_ctx.audio.player, 0));
    }
    else
    {
        qDebug("No stream!");
    }
}

void DocumentPlugin::stop()
{
    m_ctx.audio.plugin.stopEngine();
    m_stream = {};
}
}
}
