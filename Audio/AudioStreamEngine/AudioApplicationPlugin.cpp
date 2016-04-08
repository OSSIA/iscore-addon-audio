#include "AudioApplicationPlugin.hpp"
#include "AudioDocumentPlugin.hpp"
#include <core/document/Document.hpp>
#include <core/document/DocumentModel.hpp>
#include <Scenario/Application/ScenarioApplicationPlugin.hpp>
namespace Audio
{
namespace AudioStreamEngine
{

ApplicationPlugin::ApplicationPlugin(const iscore::ApplicationContext& app):
    iscore::GUIApplicationContextPlugin{app, "AudioApplicationPlugin", nullptr}
{

}

void ApplicationPlugin::on_newDocument(iscore::Document* doc)
{
    auto plug = new DocumentPlugin{m_ctx, *doc, &doc->model()};
    doc->model().addPluginModel(plug);

    auto& ctrl = doc->context().app.components.applicationPlugin<Scenario::ScenarioApplicationPlugin>();
    auto acts = ctrl.actions();
    for(const auto& act : acts)
    {
        if(act->objectName() == "Play")
        {
            connect(act, &QAction::toggled,
                    plug, [=] (bool b)
            { plug->play(); });
        }
        else if(act->objectName() == "Stop")
        {
            connect(act, &QAction::triggered,
                    plug, [=] (bool b) { plug->stop(); });
        }
    }
}

void ApplicationPlugin::startEngine()
{
    // Initialize libaudiostream structures
    if(m_ctx.player)
        CloseAudioPlayer(m_ctx.player);

    GetDeviceInfo(kJackRenderer, 0, &m_ctx.device_info);
    auto& dev = m_ctx.device_info;
    qDebug() << dev.fName
             << dev.fMaxInputChannels
             << dev.fMaxOutputChannels
             << dev.fDefaultBufferSize
             << dev.fDefaultSampleRate;

    m_ctx.player = OpenAudioPlayer(0, 2, 44100, 512, 65536*4, 44100*60*20, kJackRenderer, 1);
    m_ctx.renderer = GetAudioPlayerRenderer(m_ctx.player);

    GetAudioRendererInfo(m_ctx.renderer, &m_ctx.renderer_info);
}

void ApplicationPlugin::stopEngine()
{
    if(m_ctx.player)
    {
        StopAudioPlayer(m_ctx.player);
        CloseAudioPlayer(m_ctx.player);
    }
    m_ctx.player = nullptr;
    m_ctx.renderer = nullptr;
    m_ctx.device_info = DeviceInfo{};
    m_ctx.renderer_info = RendererInfo{};
}

bool ApplicationPlugin::engineStatus() const
{
    return m_ctx.player;
}

}
}
