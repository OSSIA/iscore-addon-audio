#pragma once
#include <iscore/plugins/documentdelegate/plugin/DocumentDelegatePluginModel.hpp>
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

        void play();
        void stop();

    private:
};
}
