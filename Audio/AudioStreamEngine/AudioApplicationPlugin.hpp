#pragma once
#include <iscore/plugins/application/GUIApplicationContextPlugin.hpp>
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
class ApplicationPlugin : public QObject, public iscore::GUIApplicationContextPlugin
{
        Q_OBJECT
    public:
        ApplicationPlugin(const iscore::GUIApplicationContext& app);
        ~ApplicationPlugin();

        void startEngine();
        void stopEngine();
        bool engineStatus() const;

        const AudioContext& context() const
        { return m_ctx; }

    signals:
        void audioEngineRestarting();
        void audioEngineRestarted();

    private:
        AudioContext m_ctx;

#if defined(LILV_SHARED) // TODO instead add a proper preprocessor macro that also works in static case
    public:
        Lilv::World lilv;
        std::unique_ptr<LV2GlobalContext> lv2_context;
        LV2HostContext lv2_host_context;
    private:
#endif

        void initialize() override;

        void on_createdDocument(iscore::Document& doc) override;

};
}
}
