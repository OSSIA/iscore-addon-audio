#include "AudioApplicationPlugin.hpp"
#include "AudioDocumentPlugin.hpp"
#include <core/document/Document.hpp>
#include <core/document/DocumentModel.hpp>
#include <Scenario/Application/ScenarioApplicationPlugin.hpp>
#include <Engine/ApplicationPlugin.hpp>
#include <QAction>
#include <Audio/Settings/Card/CardSettingsModel.hpp>
#include <LibAudioStreamMC++.h>
#include <iscore/tools/IdentifierGeneration.hpp>
#if defined(LILV_SHARED)

#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>
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
#include <readerwriterqueue.h>

#include <cstdio>
#include <cstdarg>
#include <boost/bimap.hpp>
#endif

//#include <faust/dsp/llvm-dsp.h>
namespace Audio
{
namespace AudioStreamEngine
{

#if defined(LILV_SHARED)

int lv2_printf(LV2_Log_Handle handle, LV2_URID type, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int r = std::vprintf(format, args);
    va_end(args);
    return r;
}
struct WorkerData
{
        LV2EffectContext& effect;

        uint32_t size;
        const void* data;
};

struct LV2GlobalContext
{
        using urid_map_t = boost::bimap<std::string, LV2_URID>;
        AudioContext& audio;
        LV2HostContext& host;

        std::vector<LV2_Options_Option> options;

        LV2_URID urid_map_cur = 1;
        urid_map_t urid_map;

        LV2_URID_Map map{this,
            [] (auto ptr, auto val)
            {
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
            }
        };

        LV2_URID_Unmap unmap{this,
            [] (auto ptr, auto val)
            {
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
            }
        };

        LV2_Event_Feature event{this,
            [] (auto ptr, auto cb) -> uint32_t
            {
                ISCORE_TODO;
                return 0;
            },
            [] (auto ptr, auto cb) -> uint32_t
            {
                ISCORE_TODO;
                return 0;
            }
        };


        LV2_Worker_Schedule worker{this,
            [ ] (auto ptr, uint32_t s, const void* data)
            {
                auto& c = *static_cast<LV2GlobalContext*>(ptr);
                LV2EffectContext* cur = c.host.current;
                if(cur->worker && !cur->worker_response)
                {
                    auto& w = *cur->worker;
                    if(w.work)
                    {
                        w.work(
                            cur->instance->lv2_handle,
                            [] (LV2_Worker_Respond_Handle sub_h, uint32_t sub_s, const void* sub_d)
                            {
                            return LV2_WORKER_ERR_UNKNOWN;
                                auto sub_c = static_cast<LV2EffectContext*>(sub_h);
                                sub_c->worker_data.resize(sub_s);
                                std::copy_n((const char*) sub_d, sub_s, sub_c->worker_data.data());

                                sub_c->worker_response = true;
                                return LV2_WORKER_SUCCESS;
                            }
                            , cur, s, data);
                        return LV2_WORKER_SUCCESS;
                    }
                }
                return LV2_WORKER_ERR_UNKNOWN;
            }
        };
        LV2_Worker_Schedule worker_state{this,
            [ ] (auto ptr, auto, auto)
            {
                return LV2_WORKER_ERR_UNKNOWN;
            }
        };

        LV2_Log_Log logger{this, lv2_printf,
            [] (auto h, auto t, auto fmt, auto lst)
            {
                return std::vprintf(fmt, lst);
            }
        };

        LV2_Extension_Data_Feature ext_data{nullptr};


        LV2_Resize_Port_Resize ext_port_resize{this,
            [] (LV2_Resize_Port_Feature_Data data, uint32_t index, size_t size)
            {
                return LV2_RESIZE_PORT_ERR_UNKNOWN;
            }
        };

        LV2_State_Make_Path state_make_path{this,
            [] (LV2_State_Make_Path_Handle, const char*) -> char*
            {
                ISCORE_TODO;
                return nullptr;
            }
        };

        LV2_Feature map_feature{LV2_URID__map, &map};
        LV2_Feature unmap_feature{LV2_URID__unmap, &unmap};
        LV2_Feature event_feature{LV2_EVENT_URI, &event};
        LV2_Feature options_feature{LV2_OPTIONS__options, nullptr};
        LV2_Feature worker_feature{LV2_WORKER__schedule, &worker};
        LV2_Feature worker_state_feature{LV2_WORKER__schedule, &worker};
        LV2_Feature logger_feature{LV2_LOG__log, &logger};
        LV2_Feature ext_data_feature{LV2_DATA_ACCESS_URI, &ext_data};
        LV2_Feature ext_port_resize_feature{LV2_RESIZE_PORT_URI, &ext_port_resize};
        LV2_Feature state_make_path_feature{LV2_STATE__makePath, &state_make_path};

        LV2_Feature state_load_default_feature{LV2_STATE__loadDefaultState, nullptr};
        LV2_Feature state_thread_safe_restore_feature{LV2_STATE__threadSafeRestore, nullptr};

        LV2_Feature bounded{LV2_BUF_SIZE__boundedBlockLength, nullptr};
        LV2_Feature pow2{LV2_BUF_SIZE__powerOf2BlockLength, nullptr};

        std::vector<LV2_Feature*> lv2_features;

        LV2GlobalContext(AudioContext& a, LV2HostContext& host):
            audio{a},
            host{host}
        {
            options.reserve(8);

            options.push_back(LV2_Options_Option{
                        LV2_OPTIONS_INSTANCE,
                        0,
                        map.map(map.handle, LV2_BUF_SIZE__minBlockLength),
                        sizeof(a.buffer_size),
                        map.map(map.handle, LV2_ATOM__Int),
                        &a.buffer_size
                        });
            options.push_back(LV2_Options_Option{
                        LV2_OPTIONS_INSTANCE,
                        0,
                        map.map(map.handle, LV2_BUF_SIZE__maxBlockLength),
                        sizeof(a.buffer_size),
                        map.map(map.handle, LV2_ATOM__Int),
                        &a.buffer_size
                        });
            options.push_back(LV2_Options_Option{
                        LV2_OPTIONS_INSTANCE,
                        0,
                        map.map(map.handle, LV2_CORE__sampleRate),
                        sizeof(audio.sample_rate),
                        map.map(map.handle, LV2_ATOM__Double),
                        &audio.sample_rate
                        });
            options.push_back(LV2_Options_Option{
                        LV2_OPTIONS_INSTANCE,
                        0,
                        map.map(map.handle, LV2_BUF_SIZE__sequenceSize),
                        sizeof(host.midi_buffer_size),
                        map.map(map.handle, LV2_ATOM__Int),
                        &host.midi_buffer_size
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
            // lv2_features.push_back(&ext_data_feature);
            // lv2_features.push_back(&ext_port_resize_feature);
            // lv2_features.push_back(&state_make_path_feature);
            // lv2_features.push_back(&state_load_default_feature);
            // lv2_features.push_back(&state_thread_safe_restore_feature);
            lv2_features.push_back(&bounded);
            lv2_features.push_back(&pow2);
            lv2_features.push_back(nullptr); // must be a null-terminated array per LV2 API.

        }
};
#endif

ApplicationPlugin::ApplicationPlugin(const iscore::ApplicationContext& app):
    iscore::ApplicationPlugin{app},
    m_ctx{*this}
  #if defined(LILV_SHARED)
  , lv2_context{std::make_unique<LV2GlobalContext>(m_ctx, lv2_host_context)}
  , lv2_host_context{nullptr, lv2_context->lv2_features.data(), *lilv.me}
  #endif
{
#if defined(LILV_SHARED) // TODO instead add a proper preprocessor macro that also works in static case
    lilv.load_all();

    auto& map = lv2_context->map;
    // Atom stuff
    lv2_host_context.null_id = map.map(map.handle, "");
    lv2_host_context.midi_event_id = map.map(map.handle, LILV_URI_MIDI_EVENT);
    lv2_host_context.atom_sequence_id = map.map(map.handle, LV2_ATOM__Sequence);
    lv2_host_context.atom_chunk_id = map.map(map.handle, LV2_ATOM__Chunk);
    lv2_atom_forge_init(&lv2_host_context.forge, &map);
#endif
}

void ApplicationPlugin::initialize()
{
    // Restart everything if audio settings change.
    auto& set = iscore::ApplicationPlugin::context.settings<Settings::Model>();
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

    auto& stngs = iscore::ApplicationPlugin::context.settings<Audio::Settings::Model>();
    auto api = stngs.getDriverId();
    if(api == -1)
        return;

    // Initialize libaudiostream structures
    auto card = CardIdFromString(api, stngs.getCard());
    if(card == -1)
    {
      int n = GetDeviceCount(api);
      if(n == 0)
        return;
      card = 0;
    }

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
    m_ctx.buffer_size = stngs.getBufferSize();
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


GUIApplicationPlugin::GUIApplicationPlugin(const iscore::GUIApplicationContext& app):
  iscore::GUIApplicationPlugin{app}
{

}

void GUIApplicationPlugin::on_createdDocument(iscore::Document& doc)
{
    auto& audio_ctx = iscore::GUIApplicationPlugin::context.applicationPlugin<Audio::AudioStreamEngine::ApplicationPlugin>().context() ;
    doc.model().addPluginModel(new DocumentPlugin{audio_ctx, doc.context(), getStrongId(doc.model().pluginModels()), &doc.model()});
}

}
}
