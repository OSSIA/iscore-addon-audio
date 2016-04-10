#pragma once
#include <iscore/plugins/documentdelegate/plugin/DocumentDelegatePluginModel.hpp>
#include <core/document/Document.hpp>
#include <Audio/AudioStreamEngine/Context.hpp>
#include <Process/TimeValue.hpp>
namespace Audio
{
namespace AudioStreamEngine
{
class DocumentPlugin : public iscore::DocumentPlugin
{
    public:
        DocumentPlugin(
                const AudioContext& ctx,
                iscore::Document& doc,
                QObject* parent):
            iscore::DocumentPlugin{doc, "AudioDocumentPlugin", parent},
            m_ctx{doc.context(), ctx}
        {

        }

        void play();
        void stop();

    private:
        Context m_ctx;
        AudioStream m_stream;
};
}
}
