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
class ReturnComponent final : public ProcessComponent_T<Return::ProcessModel>
{
       COMPONENT_METADATA(Audio::AudioStreamEngine::ReturnComponent)

        using system_t = Audio::AudioStreamEngine::DocumentPlugin;
    public:
       ReturnComponent(
               const Id<Component>& id,
               Return::ProcessModel& sound,
               const system_t& doc,
               const iscore::DocumentContext& ctx,
               QObject* parent_obj);


       AudioStream makeStream(const Context& ctx) const override;
};
}
}
