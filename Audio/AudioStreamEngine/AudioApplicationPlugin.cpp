#include "AudioApplicationPlugin.hpp"
#include "AudioDocumentPlugin.hpp"
#include <core/document/Document.hpp>
#include <core/document/DocumentModel.hpp>
#include <Scenario/Application/ScenarioApplicationPlugin.hpp>
#include <Engine/ApplicationPlugin.hpp>
#include <QAction>
#include <Audio/Settings/Card/CardSettingsModel.hpp>
#include <LibAudioStreamMC++.h>
#include <iscore/tools/SettableIdentifierGeneration.hpp>
#if defined(LILV_SHARED)

#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/buf-size/buf-size.h>
#include <lv2/lv2plug.in/ns/ext/data-access/data-access.h>
#include <lv2/lv2plug.in/ns/ext/dynmanifest/dynmanifest.h>
#include <lv2/lv2plug.in/ns/ext/event/event.h>
#include <lv2/lv2plug.in/ns/ext/instance-access/instance-access.h>
#include <lv2/lv2plug.in/ns/ext/log/log.h>
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>
#include <lv2/lv2plug.in/ns/ext/morph/morph.h>
#include <lv2/lv2plug.in/ns/ext/options/options.h>
#include <lv2/lv2plug.in/ns/ext/parameters/parameters.h>
#include <lv2/lv2plug.in/ns/ext/patch/patch.h>
#include <lv2/lv2plug.in/ns/ext/port-groups/port-groups.h>
#include <lv2/lv2plug.in/ns/ext/port-props/port-props.h>
#include <lv2/lv2plug.in/ns/ext/presets/presets.h>
#include <lv2/lv2plug.in/ns/ext/resize-port/resize-port.h>
#include <lv2/lv2plug.in/ns/ext/state/state.h>
#include <lv2/lv2plug.in/ns/ext/time/time.h>
#include <lv2/lv2plug.in/ns/ext/uri-map/uri-map.h>
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>
#include <lv2/lv2plug.in/ns/ext/worker/worker.h>

#include <Audio/AudioStreamEngine/Streams/AudioStreamIScoreExtensions.h>
#include <cstdio>
#include <cstdarg>
#include <boost/bimap.hpp>
#endif

//#include <faust/dsp/llvm-dsp.h>
namespace Audio
{
namespace AudioStreamEngine
{

int lv2_printf(LV2_Log_Handle handle, LV2_URID type, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int r = std::vprintf(format, args);
    va_end(args);
    return r;
}

struct LV2GlobalContext
{
        using urid_map_t = boost::bimap<std::string, LV2_URID>;
        LV2_URID urid_map_cur = 1;
        urid_map_t urid_map;
        LV2_URID_Map map{this, [] (auto ptr, auto val) {
                auto& c = *static_cast<LV2GlobalContext*>(ptr);
                auto& map = c.urid_map.left;

                std::string v{val};
                auto it = map.find(v);
                if(it != map.end())
                {
                    return it->second;
                }
                else
                {
                    auto n = c.urid_map_cur;
                    map.insert(std::make_pair(v, n));
                    c.urid_map_cur++;
                    return n;
                }
            }};
        LV2_URID_Unmap unmap{this, [] (auto ptr, auto val) {
                auto& c = *static_cast<LV2GlobalContext*>(ptr);
                auto& map = c.urid_map.right;

                auto it = map.find(val);
                if(it != map.end())
                {
                    return it->second.data();
                }
                else
                {
                    return (const char*) nullptr;
                }
            }};

        LV2_Event_Feature event{this,
                                [] (auto ptr, auto cb) -> uint32_t { ISCORE_TODO; return 0; },
                                [] (auto ptr, auto cb) -> uint32_t { ISCORE_TODO; return 0; }
                               };
        std::vector<LV2_Options_Option> options;

        int32_t block = 512;

        double srt = 44100;

        LV2_Worker_Schedule worker{this, [ ] (auto ptr, auto, auto) { return LV2_WORKER_ERR_UNKNOWN; }};

        LV2_Log_Log logger{this, lv2_printf, [] (auto h, auto t, auto fmt, auto lst) { return std::vprintf(fmt, lst); } };

        LV2_Extension_Data_Feature ext_data{[] (auto dat) -> const void* { return nullptr; }};

        LV2_Resize_Port_Resize ext_port_resize{this, [] (LV2_Resize_Port_Feature_Data data, uint32_t index, size_t size) { return LV2_RESIZE_PORT_ERR_UNKNOWN; }};

        LV2_Feature map_feature{LV2_URID__map, &map};
        LV2_Feature unmap_feature{LV2_URID__unmap, &unmap};
        LV2_Feature event_feature{LV2_EVENT_URI, &event};
        LV2_Feature options_feature{LV2_OPTIONS__options, nullptr};
        LV2_Feature worker_feature{LV2_WORKER__schedule, &worker};
        LV2_Feature logger_feature{LV2_LOG__log, &logger};
        LV2_Feature ext_data_feature{LV2_DATA_ACCESS_URI, &ext_data};
        LV2_Feature ext_port_resize_feature{LV2_RESIZE_PORT_URI, &ext_port_resize};

        std::vector<LV2_Feature*> lv2_features;

        LV2GlobalContext()
        {
            options.reserve(8);

            options.push_back(LV2_Options_Option{
                        LV2_OPTIONS_INSTANCE,
                        0,
                        map.map(map.handle, LV2_BUF_SIZE__minBlockLength),
                        sizeof(block),
                        map.map(map.handle, LV2_ATOM__Int),
                        &block
                        });
            options.push_back(LV2_Options_Option{
                        LV2_OPTIONS_INSTANCE,
                        0,
                        map.map(map.handle, LV2_BUF_SIZE__maxBlockLength),
                        sizeof(block),
                        map.map(map.handle, LV2_ATOM__Int),
                        &block
                        });
            options.push_back(LV2_Options_Option{
                        LV2_OPTIONS_INSTANCE,
                        0,
                        map.map(map.handle, LV2_BUF_SIZE__boundedBlockLength),
                        sizeof(block),
                        map.map(map.handle, LV2_ATOM__Int),
                        &block
                        });
            options.push_back(LV2_Options_Option{
                        LV2_OPTIONS_INSTANCE,
                        0,
                        map.map(map.handle, LV2_CORE__sampleRate),
                        sizeof(srt),
                        map.map(map.handle, LV2_ATOM__Double),
                        &srt
                        });

            options.push_back(LV2_Options_Option{
                        LV2_OPTIONS_INSTANCE,
                        0,
                        0,
                        0,
                        0,
                        nullptr
                        });

            options_feature.data = options.data();


            lv2_features.push_back(&map_feature);
            lv2_features.push_back(&unmap_feature);
            lv2_features.push_back(&event_feature);
            lv2_features.push_back(&options_feature);
            lv2_features.push_back(&worker_feature);
            lv2_features.push_back(&logger_feature);
            lv2_features.push_back(&ext_data_feature);
            lv2_features.push_back(&ext_port_resize_feature);
            lv2_features.push_back(nullptr); // must be a null-terminated array per LV2 API.
        }
};

ApplicationPlugin::ApplicationPlugin(const iscore::GUIApplicationContext& app):
    iscore::GUIApplicationContextPlugin{app},
    m_ctx{*this}
  #if defined(LILV_SHARED)
  , lv2_context{std::make_unique<LV2GlobalContext>()}
  , lv2_host_context{nullptr, lv2_context->lv2_features.data(), *lilv.me}
  #endif
{
#if defined(LILV_SHARED) // TODO instead add a proper preprocessor macro that also works in static case
    lilv.load_all();
#endif
}

void ApplicationPlugin::initialize()
{
    // Restart everything if audio settings change.
    auto& set = GUIApplicationContextPlugin::context.settings<Settings::Model>();
    con(set, &Settings::Model::BufferSizeChanged,
        this, &ApplicationPlugin::startEngine);
    con(set, &Settings::Model::CardChanged,
        this, &ApplicationPlugin::startEngine);
    con(set, &Settings::Model::DriverChanged,
        this, &ApplicationPlugin::startEngine);
    con(set, &Settings::Model::RateChanged,
        this, &ApplicationPlugin::startEngine);
    startEngine();

   // startMTDSPFactories();
}

ApplicationPlugin::~ApplicationPlugin()
{
    stopEngine();
    //stopMTDSPFactories();
}

void ApplicationPlugin::on_createdDocument(iscore::Document& doc)
{
    doc.model().addPluginModel(new DocumentPlugin{m_ctx, doc, getStrongId(doc.model().pluginModels()), &doc.model()});
}

static int CardIdFromString(int api, const QString& str);
static int CardIdFromString(int api, const QString& str)
{
    int n = GetDeviceCount(api);
    for(int i = 0; i < n; ++i)
    {
        DeviceInfo devinfo;
        GetDeviceInfo(api, i, &devinfo);
        if(str == QString::fromUtf8(devinfo.fName))
        {
            return i;
        }
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

    qDebug() << "opening audio with: " << stngs.getRate() << stngs.getBufferSize();
    AudioGlobalsInit(2, 2, stngs.getRate(),
                     stngs.getBufferSize(),
                     65536*4,
                     0,
                     1);

    m_ctx.renderer = MakeAudioRenderer(api);
    m_ctx.sample_rate = stngs.getRate();
    GetAudioRendererInfo(m_ctx.renderer, &m_ctx.renderer_info);
    OpenAudioRenderer(m_ctx.renderer, card, card, 2, 2, stngs.getBufferSize(), stngs.getRate());

    emit audioEngineRestarted();
}

void ApplicationPlugin::stopEngine()
{
    emit audioEngineRestarting();
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
