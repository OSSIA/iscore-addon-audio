#include "SoundComponent.hpp"
#include <Audio/SoundProcess/SoundProcessModel.hpp>
#include <3rdparty/libaudiostream/src/LibAudioStreamMC++.h>
namespace Audio
{
namespace AudioStreamEngine
{

SoundComponent::SoundComponent(
        const Id<iscore::Component>& id,
        Sound::ProcessModel& sound,
        const SoundComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent_obj):
    ProcessComponent_T{sound, id, "SoundComponent", parent_obj}
{

}

AudioStream SoundComponent::makeStream(const Context& ctx) const
{
    auto& sound = process().file();
    /*
    m_buffers.clear();
    m_buffers.push_back(());
    MakeBufferSound(float** buffer, long length, long channels, bool clear);;
    */
    return MakeReadSound(sound.name().toLocal8Bit().constData());
}

}
}
