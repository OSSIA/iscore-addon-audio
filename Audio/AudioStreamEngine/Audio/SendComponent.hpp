#pragma once
#include <Audio/AudioStreamEngine/Scenario/ProcessComponent.hpp>

namespace Audio
{
namespace Send
{
class ProcessModel;
}
namespace AudioStreamEngine
{
class SendComponent final : public ProcessComponent_T<Send::ProcessModel>
{
       COMPONENT_METADATA(Audio::AudioStreamEngine::SendComponent)

        using system_t = Audio::AudioStreamEngine::DocumentPlugin;
    public:
       SendComponent(
               const Id<Component>& id,
               Send::ProcessModel& sound,
               const system_t& doc,
               const iscore::DocumentContext& ctx,
               QObject* parent_obj);


       AudioStream makeStream(const Context& ctx) const override;
};
}
}
