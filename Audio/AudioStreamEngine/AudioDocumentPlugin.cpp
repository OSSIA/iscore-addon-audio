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

void DocumentPlugin::play()
{
    // First find the root constraint
    auto doc = dynamic_cast<Scenario::ScenarioDocumentModel*>(&m_ctx.doc.document.model().modelDelegate());
    if(!doc)
        return;

    if(!m_ctx.audio.plugin.engineStatus())
        return;

    // Reset the player
    stop();
    openPlayer();

    // Create our tree
    m_comp = new ConstraintComponent(
                getStrongId(doc->baseConstraint().components),
                doc->baseConstraint(),
                *this,
                m_ctx.doc,
                this);
    doc->baseConstraint().components.add(m_comp);
    AudioDependencyGraph graph{*m_comp};
    if(auto sorted_vertices = graph.check())
    {
        graph.apply(*sorted_vertices, m_ctx);
    }
    else
    {
        stop();
        return;
    }

    con(m_ctx.doc.document, &iscore::Document::aboutToClose,
        this, [=] () {
        // Stop
        stop();

        // Delete
        doc->baseConstraint().components.remove(m_comp->id());
    });


    // First we have to construct our graph

    // If the graph is acyclic, we can walk it and toposort it to
    // create the streams
    // finnally, we can associate the streams with their elements.

    // Play

    // TODO make id from components !!!!
    if(m_comp)
    {
        m_stream = m_comp->getStream();
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

    StartAudioPlayer(m_ctx.audio.player);
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
        if(m_comp)
        {
            doc->baseConstraint().components.remove(m_comp->id());
            m_comp = nullptr;
        }
    }

    m_stream = {};
}

void DocumentPlugin::openPlayer()
{
    if(!m_ctx.audio.player)
    {
        m_ctx.audio.player = OpenAudioClient(m_ctx.audio.renderer);
    }
}

}
}
