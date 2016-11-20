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
        AudioContext(ApplicationPlugin& p):
            plugin{p}
        {

        }

        ApplicationPlugin& plugin;

        AudioPlayerPtr player{};
        AudioRendererPtr renderer{};
        DeviceInfo device_info{};
        RendererInfo renderer_info{};

        double sample_rate = 0;
        int32_t buffer_size = 0;
};

struct Context
{
      Context(const iscore::DocumentContext& d, AudioContext& a) :
        doc(d),
        audio(a)
      {

    }

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
