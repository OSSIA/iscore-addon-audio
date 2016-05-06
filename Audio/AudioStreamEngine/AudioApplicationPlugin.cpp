#include "AudioApplicationPlugin.hpp"
#include "AudioDocumentPlugin.hpp"
#include <core/document/Document.hpp>
#include <core/document/DocumentModel.hpp>
#include <Scenario/Application/ScenarioApplicationPlugin.hpp>
#include <OSSIA/OSSIAApplicationPlugin.hpp>
#include <QAction>
#include <Audio/Settings/Card/CardSettingsModel.hpp>
#include <LibAudioStreamMC++.h>
#include <faust/dsp/llvm-dsp.h>
namespace Audio
{
namespace AudioStreamEngine
{

ApplicationPlugin::ApplicationPlugin(const iscore::ApplicationContext& app):
    iscore::GUIApplicationContextPlugin{app, "AudioApplicationPlugin", nullptr},
    m_ctx{*this}
{
    startMTDSPFactories();
}

ApplicationPlugin::~ApplicationPlugin()
{
    stopMTDSPFactories();
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

void ApplicationPlugin::on_loadedDocument(iscore::Document* doc)
{
    on_newDocument(doc);
}

static int CardIdFromString(int api, const QString& str);
static int CardIdFromString(int api, const QString& str)
{
    for(int i = 0; i < GetDeviceCount(api); ++i)
    {
        DeviceInfo devinfo;
        GetDeviceInfo(api, i, &devinfo);
        if(str == QString::fromUtf8(devinfo.fName))
            return i;
    }
    return -1;
}

void ApplicationPlugin::startEngine()
{
    stopEngine();

    auto& stngs = iscore::GUIApplicationContextPlugin::context.settings<Audio::Settings::Model>();
    auto api = stngs.getDriverId();
    if(api == -1)
        return;

    // Initialize libaudiostream structures
    auto card = CardIdFromString(api, stngs.getCard());
    if(card == -1)
        return;

    GetDeviceInfo(api, card, &m_ctx.device_info);
    auto& dev = m_ctx.device_info;
    qDebug() << dev.fName
             << dev.fMaxInputChannels
             << dev.fMaxOutputChannels
             << dev.fDefaultBufferSize
             << dev.fDefaultSampleRate;

    AudioGlobalsInit(2, 2, stngs.getRate(),
                     stngs.getBufferSize(),
                     65536*4,
                     stngs.getRate()*60*20,
                     1);

    m_ctx.renderer = MakeAudioRenderer(api);
    GetAudioRendererInfo(m_ctx.renderer, &m_ctx.renderer_info);
    OpenAudioRenderer(m_ctx.renderer, 0, card, 2, 2, stngs.getBufferSize(), stngs.getRate());

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
