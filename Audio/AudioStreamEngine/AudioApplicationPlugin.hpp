#pragma once
#include <iscore/plugins/application/GUIApplicationPlugin.hpp>
#include <Audio/AudioStreamEngine/Context.hpp>

#if defined(LILV_SHARED) // TODO instead add a proper preprocessor macro that also works in static case
#include <lilv/lilvmm.hpp>
#include <Audio/AudioStreamEngine/Streams/LV2Context.hpp>
#endif
struct LV2HostContext;
namespace Audio
{
namespace AudioStreamEngine
{
struct LV2GlobalContext;
class ApplicationPlugin : public QObject, public iscore::ApplicationPlugin
{
        Q_OBJECT
    public:
        ApplicationPlugin(const iscore::ApplicationContext& app);
        ~ApplicationPlugin();

        void startEngine();
        void stopEngine();
        bool engineStatus() const;

        AudioContext& context()
        { return m_ctx; }
        const AudioContext& context() const
        { return m_ctx; }

    signals:
        void audioEngineRestarting();
        void audioEngineRestarted();

    private:
        void initialize() override;

        AudioContext m_ctx;

#if defined(LILV_SHARED) // TODO instead add a proper preprocessor macro that also works in static case
    public:
        Lilv::World lilv;
        std::unique_ptr<LV2GlobalContext> lv2_context;
        LV2HostContext lv2_host_context;
#endif

};

class GUIApplicationPlugin : public iscore::GUIApplicationPlugin
{
public:
  GUIApplicationPlugin(const iscore::GUIApplicationContext& app);
  void on_createdDocument(iscore::Document& doc) override;

};
}
}
