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

    public:
            SoundComponent(
                Sound::ProcessModel& sound,
                DocumentPlugin& doc,
                const Id<Component>& id,
                QObject* parent_obj);

        void makeStream(const Context& ctx) override;
};
AUDIO_PROCESS_COMPONENT_FACTORY(SoundComponentFactory, "c5a44139-49af-4374-a0e4-9ed767de18e5", SoundComponent, Sound::ProcessModel)
}
}
