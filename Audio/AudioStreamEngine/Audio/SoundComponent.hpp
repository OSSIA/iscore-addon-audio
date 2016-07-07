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
class SoundComponent final :
        public ProcessComponent_T<Sound::ProcessModel, false, true>
{
        COMPONENT_METADATA("12289f16-36ac-4900-afc8-18de63cdff89")

        using system_t = Audio::AudioStreamEngine::DocumentPlugin;
    public:
       SoundComponent(
               const Id<Component>& id,
               Sound::ProcessModel& sound,
               const system_t& doc,
               const iscore::DocumentContext& ctx,
               QObject* parent_obj);


       void makeStream(const Context& ctx) override;

    private:
};
AUDIO_COMPONENT_FACTORY(SoundComponentFactory, "c5a44139-49af-4374-a0e4-9ed767de18e5", SoundComponent, Sound::ProcessModel)
}
}
