#include "EffectComponent.hpp"
#include <Audio/EffectProcess/EffectProcessModel.hpp>

namespace Audio
{
namespace AudioStreamEngine
{

EffectComponent::EffectComponent(
        const Id<iscore::Component>& id,
        Effect::ProcessModel& sound,
        const EffectComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent_obj):
    ProcessComponent_T{sound, id, "EffectComponent", parent_obj}
{

}

void EffectComponent::makeStream(const Context& ctx)
{
}

}
}
