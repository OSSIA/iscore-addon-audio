#pragma once
#include <Audio/AudioStreamEngine/Scenario/ProcessComponent.hpp>

namespace Audio
{
namespace Mix
{
class ProcessModel;
}
namespace AudioStreamEngine
{
class MixComponent final : public ProcessComponent_T<Mix::ProcessModel>
{
       COMPONENT_METADATA(Audio::AudioStreamEngine::MixComponent)

        using system_t = Audio::AudioStreamEngine::DocumentPlugin;
    public:
       MixComponent(
               const Id<Component>& id,
               Mix::ProcessModel& sound,
               const system_t& doc,
               const iscore::DocumentContext& ctx,
               QObject* parent_obj);


       void makeStream(const Context& ctx) override;
};
}
}
