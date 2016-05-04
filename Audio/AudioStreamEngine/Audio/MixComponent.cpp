#include "MixComponent.hpp"
#include <Audio/MixProcess/MixProcessModel.hpp>

namespace Audio
{
namespace AudioStreamEngine
{

MixComponent::MixComponent(
        const Id<iscore::Component>& id,
        Mix::ProcessModel& sound,
        const MixComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent_obj):
    ProcessComponent_T{sound, id, "MixComponent", parent_obj}
{

}

void MixComponent::makeStream(const Context& ctx)
{
}

}
}
