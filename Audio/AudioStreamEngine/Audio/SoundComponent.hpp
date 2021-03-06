#pragma once
#include <Audio/AudioStreamEngine/Scenario/ProcessComponent.hpp>
#include <Media/Sound/SoundModel.hpp>
namespace Audio
{
namespace Sound = Media::Sound;
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
                const Id<score::Component>& id,
                QObject* parent_obj);

        void makeStream(const Context& ctx) override;

};

using SoundComponentFactory = ProcessComponentFactory_T<SoundComponent>;
}
}
