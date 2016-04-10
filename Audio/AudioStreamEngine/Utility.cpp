#include "Utility.hpp"
#include <algorithm>
namespace Audio
{
namespace AudioStreamEngine
{

AudioStream makeNStreamsParallel(const std::vector<AudioStream>& streams)
{
    switch(streams.size())
    {
        case 0:
            return {};
            break;
        case 1:
            return streams[0];
            break;
        case 2:
            return MakeMixSound(streams[0], streams[1]);
            break;
        default:
            return std::accumulate(
                        ++streams.begin(),
                        streams.end(),
                        streams[0],
                    [] (AudioStream a1, AudioStream a2) {
                return MakeMixSound(a1, a2);
            });
            break;
    }
}

}
}
