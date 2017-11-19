#pragma once
#include <score/plugins/documentdelegate/plugin/DocumentPlugin.hpp>
#include <core/document/Document.hpp>
#include <Audio/AudioStreamEngine/Context.hpp>
#include <Process/TimeValue.hpp>
#include <Audio/AudioStreamEngine/AudioGraphNode.hpp>
#include <score_plugin_audio_export.h>
namespace Scenario
{
class IntervalModel;
}
namespace Audio
{
namespace AudioStreamEngine
{
class Interval;
class SCORE_PLUGIN_AUDIO_EXPORT DocumentPlugin : public score::DocumentPlugin
{
    public:
        DocumentPlugin(
                AudioContext& ctx,
                const score::DocumentContext& doc,
                Id<score::DocumentPlugin> id,
                QObject* parent);
        ~DocumentPlugin();

        Context audioContext;

        audio_frame_t toFrame(TimeVal t) const
        {
            return t.msec() * audioContext.audio.sample_rate / 1000.0;
        }


        bool offsetting = false;
        AudioStream makeStream(Scenario::IntervalModel& cst);
        void stop();

        void on_documentClosing() override;
    private:
        void openPlayer();

        Interval* m_comp{};
};

}
}
