#pragma once
#include <iscore/plugins/documentdelegate/plugin/DocumentDelegatePluginModel.hpp>
#include <Audio/AudioEngine.hpp>
namespace Audio
{
class AudioDocumentPlugin : public iscore::DocumentPlugin
{
    public:
        AudioDocumentPlugin(
                iscore::Document& doc,
                QObject* parent):
            iscore::DocumentPlugin{doc, "AudioDocumentPlugin", parent}
        {

        }

        AudioEngine& engine() { return m_engine; }

    private:
        AudioEngine m_engine;
};
}
