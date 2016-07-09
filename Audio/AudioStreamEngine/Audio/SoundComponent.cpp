#include "SoundComponent.hpp"
#include <Audio/SoundProcess/SoundProcessModel.hpp>
#include <LibAudioStreamMC++.h>
#include <Audio/AudioStreamEngine/Streams/AudioStreamIScoreExtensions.h>
namespace Audio
{
namespace AudioStreamEngine
{

SoundComponent::SoundComponent(
        const Id<iscore::Component>& id,
        Sound::ProcessModel& sound,
        const SoundComponent::system_t& doc,
        QObject* parent_obj):
    ProcessComponent_T{sound, id, "SoundComponent", parent_obj}
{

}

void SoundComponent::makeStream(const Context& ctx)
{
    auto& path = process().file();
    auto read_sound = MakeReadSound(path.name().toLocal8Bit().constData());

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


    // TODO optimize me by instead getting the already loaded data.
}

}
}
