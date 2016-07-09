#pragma once
#include <Audio/AudioStreamEngine/Scenario/ProcessComponent.hpp>

namespace Audio
{
namespace Return
{
class ProcessModel;
}
namespace AudioStreamEngine
{
class ReturnComponent final :
        public ProcessComponent_T<Return::ProcessModel, false, true>
{
        COMPONENT_METADATA("11770cd3-dc49-4d4a-93b2-ab2e5e7faec4")

        using system_t = Audio::AudioStreamEngine::DocumentPlugin;
    public:
       ReturnComponent(
               const Id<Component>& id,
               Return::ProcessModel& sound,
               const system_t& doc,
               QObject* parent_obj);


       void makeStream(const Context& ctx) override;
};

AUDIO_PROCESS_COMPONENT_FACTORY(ReturnComponentFactory, "f8d4cbe3-b187-43ed-8831-2510b4cffb01", ReturnComponent, Return::ProcessModel)
}
}
