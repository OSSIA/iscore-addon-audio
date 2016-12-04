#pragma once
#include <iscore/plugins/documentdelegate/plugin/DocumentPlugin.hpp>
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
                Id<iscore::DocumentPlugin> id,
                QObject* parent):
            iscore::DocumentPlugin{doc.context(), std::move(id), "AudioDocumentPlugin", parent},
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
