#pragma once
#include <LibAudioStreamMC++.h>
#include <Process/TimeValue.hpp>

namespace iscore
{
struct DocumentContext;
}
namespace Audio
{
namespace AudioStreamEngine
{
class ApplicationPlugin;

struct AudioContext
{
        ApplicationPlugin& plugin;

        AudioPlayerPtr player{};
        AudioRendererPtr renderer{};
        DeviceInfo device_info{};
        RendererInfo renderer_info{};

        int sample_rate = 0;
};

struct Context
{
        const iscore::DocumentContext& doc;
        AudioContext& audio;

        long time(const TimeValue& t)
        {
            return t.msec() * time_factor;
        }

        const double time_factor{};
};

}
}
