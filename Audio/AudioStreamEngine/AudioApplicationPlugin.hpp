#pragma once
#include <iscore/plugins/application/GUIApplicationContextPlugin.hpp>
#include <Audio/AudioStreamEngine/Context.hpp>

namespace Audio
{
namespace AudioStreamEngine
{
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
        void audioEngineRestarted();

    private:
        AudioContext m_ctx;

        void initialize() override;

        void on_createdDocument(iscore::Document& doc) override;
};
}
}
