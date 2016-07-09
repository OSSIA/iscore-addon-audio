#include "AudioDocumentPlugin.hpp"
#include <Scenario/Process/ScenarioModel.hpp>
#include <Loop/LoopProcessModel.hpp>
#include <map>
#include <Scenario/Process/Algorithms/Accessors.hpp>
#include <core/document/Document.hpp>
#include <core/document/DocumentModel.hpp>

#include <Scenario/Document/ScenarioDocument/ScenarioDocumentModel.hpp>
#include <Audio/AudioStreamEngine/AudioApplicationPlugin.hpp>
#include <Audio/AudioStreamEngine/AudioDependencyGraph.hpp>
#include <Audio/AudioStreamEngine/Scenario/ConstraintComponent.hpp>

namespace Audio
{
namespace AudioStreamEngine
{

AudioStream DocumentPlugin::makeStream()
{
    // First find the root constraint
    auto doc = dynamic_cast<Scenario::ScenarioDocumentModel*>(&audioContext.doc.document.model().modelDelegate());
    if(!doc)
        return nullptr;

    if(!audioContext.audio.plugin.engineStatus())
        return nullptr;

    // Reset the player
    stop();
    openPlayer();

    // Create our tree
    m_comp = new ConstraintComponent(
                getStrongId(doc->baseConstraint().components),
                doc->baseConstraint(),
                *this,
                this);
    doc->baseConstraint().components.add(m_comp);
    AudioDependencyGraph graph{*m_comp};
    if(auto sorted_vertices = graph.check())
    {
        graph.apply(*sorted_vertices, audioContext);
    }
    else
    {
        stop();
        return nullptr;
    }

    con(audioContext.doc.document, &iscore::Document::aboutToClose,
        this, [=] () {
        // Stop and clean
        stop();
    });


    if(m_comp)
    {
        return m_comp->getStream();
    }
    else
    {
        qDebug("No component!");
        return nullptr;
    }
}

void DocumentPlugin::stop()
{
    if(audioContext.audio.player)
    {
        StopAudioPlayer(audioContext.audio.player);
        CloseAudioClient(audioContext.audio.player);
        audioContext.audio.player = nullptr;
    }

    auto doc = dynamic_cast<Scenario::ScenarioDocumentModel*>(&audioContext.doc.document.model().modelDelegate());
    if(doc)
    {
        if(m_comp)
        {
            doc->baseConstraint().components.remove(m_comp->id());
            m_comp = nullptr;
        }
    }
}

void DocumentPlugin::openPlayer()
{
    if(!audioContext.audio.player)
    {
        audioContext.audio.player = OpenAudioClient(audioContext.audio.renderer);
    }
}

}
}
