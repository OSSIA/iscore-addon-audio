#pragma once
#include <3rdparty/libaudiostream/src/LibAudioStreamMC++.h>
#include <Process/TimeValue.hpp>

namespace iscore
{
class DocumentContext;
}
namespace Audio
{
namespace AudioStreamEngine
{
struct AudioContext
{
        AudioPlayerPtr player{};
        AudioRendererPtr renderer{};
        DeviceInfo device_info{};
        RendererInfo renderer_info{};
};

struct Context
{
        const iscore::DocumentContext& doc;
        const AudioContext& audio;

        long time(const TimeValue& t)
        {
            return t.msec() * time_factor;
        }

        const double time_factor{};
};

}
}
