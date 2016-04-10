#pragma once
#include <Audio/AudioStreamEngine/Scenario/ProcessComponent.hpp>

namespace Audio
{
namespace Sound
{
class ProcessModel;
}
namespace AudioStreamEngine
{
class SoundComponent final : public ProcessComponent_T<Sound::ProcessModel>
{
       COMPONENT_METADATA(Audio::AudioStreamEngine::SoundComponent)

        using system_t = Audio::AudioStreamEngine::DocumentPlugin;
    public:
       SoundComponent(
               const Id<Component>& id,
               Sound::ProcessModel& sound,
               const system_t& doc,
               const iscore::DocumentContext& ctx,
               QObject* parent_obj);


       AudioStream makeStream(const Context& ctx) const override;

    private:
       std::vector<float*> m_buffers{};
};


}
}
