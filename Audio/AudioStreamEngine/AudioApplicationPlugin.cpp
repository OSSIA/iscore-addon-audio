#include "AudioApplicationPlugin.hpp"
#include "AudioDocumentPlugin.hpp"
#include <core/document/Document.hpp>
#include <core/document/DocumentModel.hpp>
#include <Scenario/Application/ScenarioApplicationPlugin.hpp>
#include <OSSIA/OSSIAApplicationPlugin.hpp>
#include <QAction>
namespace Audio
{
namespace AudioStreamEngine
{

ApplicationPlugin::ApplicationPlugin(const iscore::ApplicationContext& app):
    iscore::GUIApplicationContextPlugin{app, "AudioApplicationPlugin", nullptr},
    m_ctx{*this}
{

}

void ApplicationPlugin::on_newDocument(iscore::Document* doc)
{
    auto plug = new DocumentPlugin{m_ctx, *doc, &doc->model()};
    doc->model().addPluginModel(plug);

    auto& ctrl = doc->context().app.components.applicationPlugin<OSSIAApplicationPlugin>();
    con(ctrl, &OSSIAApplicationPlugin::requestPlay,
            plug, [=] { plug->play(); });
    con(ctrl, &OSSIAApplicationPlugin::requestStop,
            plug, [=] { plug->stop(); });
}

void ApplicationPlugin::startEngine()
{
    stopEngine();

    // Initialize libaudiostream structures
    GetDeviceInfo(kJackRenderer, 0, &m_ctx.device_info);
    auto& dev = m_ctx.device_info;
    qDebug() << dev.fName
             << dev.fMaxInputChannels
             << dev.fMaxOutputChannels
             << dev.fDefaultBufferSize
             << dev.fDefaultSampleRate;

    AudioGlobalsInit(0, 2, 44100, 512, 65536*4, 44100*60*20, 1);
    m_ctx.renderer = MakeAudioRenderer(kJackRenderer);
    GetAudioRendererInfo(m_ctx.renderer, &m_ctx.renderer_info);
    OpenAudioRenderer(m_ctx.renderer, 0, 0, 0, 2, 512, 44100);
}

void ApplicationPlugin::stopEngine()
{
    if(m_ctx.player)
    {
        StopAudioPlayer(m_ctx.player);
        CloseAudioClient(m_ctx.player);
        m_ctx.player = nullptr;
    }

    if(m_ctx.renderer)
    {
        CloseAudioRenderer(m_ctx.renderer);
        m_ctx.renderer = nullptr;
        m_ctx.renderer_info = {};
        m_ctx.device_info = {};
    }

    AudioGlobalsDestroy();
}

bool ApplicationPlugin::engineStatus() const
{
    return m_ctx.renderer;
}

}
}
