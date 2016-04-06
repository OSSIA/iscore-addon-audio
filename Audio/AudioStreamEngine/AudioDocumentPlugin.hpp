#pragma once
#include <iscore/plugins/documentdelegate/plugin/DocumentDelegatePluginModel.hpp>
namespace Audio
{
namespace AudioStreamEngine
{
class DocumentPlugin : public iscore::DocumentPlugin
{
    public:
        DocumentPlugin(
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
}
