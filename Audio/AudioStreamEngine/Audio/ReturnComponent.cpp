#include "ReturnComponent.hpp"
#include <Audio/ReturnProcess/ReturnProcessModel.hpp>
#include <Audio/SendProcess/SendProcessModel.hpp>
#include <Audio/AudioStreamEngine/Audio/SendComponent.hpp>
#include <Audio/AudioStreamEngine/GroupAudioStream.h>

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

void ReturnComponent::makeStream(const Context& ctx)
{
    // Find send
    auto send = process().send_ptr();
    ISCORE_ASSERT(send);

    auto send_comp_it = find_if(
                send->components,
                [] (auto& comp) {
        return dynamic_cast<SendComponent*>(&comp);
    });

    ISCORE_ASSERT(send_comp_it != send->components.end());
    auto send_comp = static_cast<SendComponent*>(&(*send_comp_it));

    // Create stream from send
    m_stream = MakeSend(MakeReturn(send_comp->getStream()));
}

}
}
