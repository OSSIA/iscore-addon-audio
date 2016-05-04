#include "ReturnComponent.hpp"
#include <Audio/ReturnProcess/ReturnProcessModel.hpp>

namespace Audio
{
namespace AudioStreamEngine
{

ReturnComponent::ReturnComponent(
        const Id<iscore::Component>& id,
        Return::ProcessModel& sound,
        const ReturnComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent_obj):
    ProcessComponent_T{sound, id, "ReturnComponent", parent_obj}
{

}

AudioStream ReturnComponent::makeStream(const Context& ctx) const
{
    return {};
}

}
}
