#include "SendComponent.hpp"
#include <Audio/SendProcess/SendProcessModel.hpp>

namespace Audio
{
namespace AudioStreamEngine
{

SendComponent::SendComponent(
        const Id<iscore::Component>& id,
        Send::ProcessModel& sound,
        const SendComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent_obj):
    ProcessComponent_T{sound, id, "SendComponent", parent_obj}
{

}

AudioStream SendComponent::makeStream(const Context& ctx) const
{
    return {};
}

}
}
