#include "SoundComponent.hpp"
#include <Audio/SoundProcess/SoundProcessModel.hpp>
#include <LibAudioStreamMC++.h>
#include <Audio/AudioStreamEngine/GroupAudioStream.h>
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

void SoundComponent::makeStream(const Context& ctx)
{
    auto& sound = process().file();
    m_stream = MakeSend(MakeReadSound(sound.name().toLocal8Bit().constData()));
    // TODO optimize me by instead getting the already loaded data.
}

}
}
