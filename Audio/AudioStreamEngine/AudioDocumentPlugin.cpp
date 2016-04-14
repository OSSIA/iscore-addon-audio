#include "AudioDocumentPlugin.hpp"
#include <Scenario/Process/ScenarioModel.hpp>
#include <Loop/LoopProcessModel.hpp>
#include <map>
#include <Scenario/Process/Algorithms/Accessors.hpp>
#include <core/document/Document.hpp>
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

    qDebug() << (void*)m_ctx.audio.renderer ;
    if(!m_ctx.audio.renderer)
    {
        iscore::AppContext().components.applicationPlugin<Audio::AudioStreamEngine::ApplicationPlugin>().startEngine();
    }

    if(!m_ctx.audio.plugin.engineStatus())
        return;

    // Reset the player
    stop();

    // TODO make id from components !!!!
    startPlayer();

    // Create our tree
    auto comp = new ConstraintComponent(
                Id<iscore::Component>{1},
                doc->baseConstraint(),
                *this,
                m_ctx.doc,
                this);
    doc->baseConstraint().components.add(comp);

    con(m_ctx.doc.document, &iscore::Document::aboutToClose,
        this, [=] () {
        // Stop
        stop();

        // Delete
        doc->baseConstraint().components.remove(Id<iscore::Component>{1});
    });


    // Play

    StartAudioPlayer(m_ctx.audio.player);
    if(comp)
    {
        m_stream = comp->makeStream(
                    m_ctx);
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
    if(m_ctx.audio.player)
    {
        StopAudioPlayer(m_ctx.audio.player);
        CloseAudioClient(m_ctx.audio.player);
        m_ctx.audio.player = nullptr;
    }

    auto doc = dynamic_cast<Scenario::ScenarioDocumentModel*>(&m_ctx.doc.document.model().modelDelegate());
    if(doc)
    {
        auto it = doc->baseConstraint().components.find(Id<iscore::Component>{1});
        if(it != doc->baseConstraint().components.end())
        {
            doc->baseConstraint().components.remove(Id<iscore::Component>{1});
        }
    }

    m_stream = {};
}

void DocumentPlugin::startPlayer()
{
    if(!m_ctx.audio.player)
    {
        m_ctx.audio.player = OpenAudioClient(m_ctx.audio.renderer);
    }
}

}
}
