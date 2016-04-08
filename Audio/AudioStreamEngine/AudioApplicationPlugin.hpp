#pragma once
#include <iscore/plugins/application/GUIApplicationContextPlugin.hpp>
#include <Audio/AudioStreamEngine/Context.hpp>

namespace Audio
{
namespace AudioStreamEngine
{
class ApplicationPlugin : public QObject, public iscore::GUIApplicationContextPlugin
{
    public:
        ApplicationPlugin(const iscore::ApplicationContext& app);

        void on_newDocument(iscore::Document* doc) override;

        void startEngine();
        void stopEngine();
        bool engineStatus() const;

        const AudioContext& context() const
        { return m_ctx; }

    private:
        AudioContext m_ctx;

};
}
}
