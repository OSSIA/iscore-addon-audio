#pragma once
#include <LibAudioStreamMC++.h>
#include <vector>

namespace Audio
{
namespace AudioStreamEngine
{
AudioStream MixNStreams(
        const std::vector<AudioStream>& streams);
}
}
