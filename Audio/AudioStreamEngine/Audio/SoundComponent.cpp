#include "SoundComponent.hpp"
#include <Audio/SoundProcess/SoundProcessModel.hpp>
#include <LibAudioStreamMC++.h>
#include <Audio/AudioStreamEngine/Streams/AudioStreamIScoreExtensions.h>
#include <Audio/AudioDecoder.hpp>
namespace Audio
{
namespace AudioStreamEngine
{

SoundComponent::SoundComponent(
        Sound::ProcessModel& sound,
        DocumentPlugin& doc,
        const Id<iscore::Component>& id,
        QObject* parent_obj):
    ProcessComponent_T{sound, doc, id, "SoundComponent", parent_obj}
{

}

void SoundComponent::makeStream(const Context& ctx)
{
    auto& file = process().file();
    if(file.empty())
        return;

    auto read_sound = MakeSimpleBufferSound(file.audioData(), file.samples(), file.channels());

    // If the file is mono, we duplicate it.
    auto chan = GetChannelsSound(read_sound);
    switch(chan)
    {
        case 0:
            return;

        case 1:
            m_stream = MakeSend(
                        MakeParSound(
                            read_sound,
                            MakeCopySound(read_sound)));
            break;
        case 2:
            m_stream = MakeSend(read_sound);
            break;

        default:
        {
            // We take the first two channels for now
            std::array<long, 2> arr{0, 1};
            m_stream = MakeSend(MakeSelectSound(read_sound, arr.data(), 2));
            break;
        }
    }
}

}
}
