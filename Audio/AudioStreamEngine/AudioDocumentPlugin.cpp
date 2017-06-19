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
#include <thread>

namespace Audio
{
namespace AudioStreamEngine
{

DocumentPlugin::DocumentPlugin(AudioContext& ctx, const iscore::DocumentContext& doc, Id<iscore::DocumentPlugin> id, QObject* parent):
  iscore::DocumentPlugin{doc, std::move(id), "AudioDocumentPlugin", parent},
  audioContext{doc, ctx}
{

}

DocumentPlugin::~DocumentPlugin()
{

}

AudioStream DocumentPlugin::makeStream(Scenario::ConstraintModel& cst)
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
    m_comp = new Constraint{cst, *this, getStrongId(cst.components()), this};
    cst.components().add(m_comp);

    AudioGraphBuilder graph{m_context, *m_comp};
    if(auto sorted_vertices = graph.check())
    {
        graph.apply(*sorted_vertices, audioContext);
    }
    else
    {
        stop();
        return nullptr;
    }

    return m_comp->getStream();
}

void DocumentPlugin::stop()
{
    if(audioContext.audio.player)
    {
        StopAudioPlayer(audioContext.audio.player);
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        CloseAudioClient(audioContext.audio.player);
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        audioContext.audio.player = nullptr;
    }

    auto doc = dynamic_cast<Scenario::ScenarioDocumentModel*>(&audioContext.doc.document.model().modelDelegate());
    if(doc)
    {
        if(m_comp)
        {
            m_comp->stop();
            m_comp->constraint().components().remove(m_comp->id());
            m_comp = nullptr;
        }
    }
}

void DocumentPlugin::on_documentClosing()
{
  // Stop and clean
  stop();
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
