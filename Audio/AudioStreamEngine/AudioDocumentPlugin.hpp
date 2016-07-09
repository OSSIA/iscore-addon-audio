#pragma once
#include <iscore/plugins/documentdelegate/plugin/DocumentDelegatePluginModel.hpp>
#include <core/document/Document.hpp>
#include <Audio/AudioStreamEngine/Context.hpp>
#include <Process/TimeValue.hpp>
namespace Audio
{
namespace AudioStreamEngine
{
class ConstraintComponent;
class DocumentPlugin : public iscore::DocumentPlugin
{
    public:
        DocumentPlugin(
                AudioContext& ctx,
                iscore::Document& doc,
                QObject* parent):
            iscore::DocumentPlugin{doc.context(), "AudioDocumentPlugin", parent},
            audioContext{doc.context(), ctx}
        {

        }

        Context audioContext;

        AudioStream makeStream();
        void stop();

    private:
        void openPlayer();

        ConstraintComponent* m_comp{};
};
}
}
