#pragma once
#include <3rdparty/libaudiostream/src/LibAudioStreamMC++.h>
#include <vector>

namespace Audio
{
namespace AudioStreamEngine
{
AudioStream makeNStreamsParallel(
        const std::vector<AudioStream>& streams);
}
}
