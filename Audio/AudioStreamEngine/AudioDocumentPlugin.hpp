#pragma once
#include <iscore/plugins/documentdelegate/plugin/DocumentPlugin.hpp>
#include <core/document/Document.hpp>
#include <Audio/AudioStreamEngine/Context.hpp>
#include <Process/TimeValue.hpp>
#include <Audio/AudioStreamEngine/AudioGraphNode.hpp>
#include <iscore_plugin_audio_export.h>
namespace Scenario
{
class ConstraintModel;
}
namespace Audio
{
namespace AudioStreamEngine
{
class Constraint;
class ISCORE_PLUGIN_AUDIO_EXPORT DocumentPlugin : public iscore::DocumentPlugin
{
    public:
        DocumentPlugin(
                AudioContext& ctx,
                const iscore::DocumentContext& doc,
                Id<iscore::DocumentPlugin> id,
                QObject* parent);
        ~DocumentPlugin();

        Context audioContext;

        audio_frame_t toFrame(TimeVal t) const
        {
            return t.msec() * audioContext.audio.sample_rate / 1000.0;
        }


        bool offsetting = false;
        AudioStream makeStream(Scenario::ConstraintModel& cst);
        void stop();

        void on_documentClosing() override;
    private:
        void openPlayer();

        Constraint* m_comp{};
};

}
}
