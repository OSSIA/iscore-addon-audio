#pragma once
#include <iscore/plugins/documentdelegate/plugin/DocumentDelegatePluginModel.hpp>
#include <core/document/Document.hpp>
#include <Audio/AudioStreamEngine/Context.hpp>
#include <Process/TimeValue.hpp>
#include <Audio/AudioStreamEngine/AudioGraphNode.hpp>
namespace Audio
{
namespace AudioStreamEngine
{
class Constraint;
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

        audio_frame_t toFrame(TimeValue t) const
        {
            return t.msec() * audioContext.audio.sample_rate / 1000.0;
        }


        AudioStream makeStream();
        void stop();

    private:
        void openPlayer();

        Constraint* m_comp{};
};

}
}
