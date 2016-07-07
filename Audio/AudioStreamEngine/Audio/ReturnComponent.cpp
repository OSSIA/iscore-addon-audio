#include "ReturnComponent.hpp"
#include <Audio/ReturnProcess/ReturnProcessModel.hpp>
#include <Audio/SendProcess/SendProcessModel.hpp>
#include <Audio/AudioStreamEngine/Audio/SendComponent.hpp>
#include <Audio/AudioStreamEngine/Streams/AudioStreamIScoreExtensions.h>

namespace Audio
{
namespace AudioStreamEngine
{

// TODO CMake - factory pattern : std::tuple<Fac1, Fac2, ..., std::vector<unique_ptr<Fac>>> ; ?
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

    // Create stream from send
    m_stream = MakeSend(
                MakeReturn(
                    iscore::component<SendComponent>(send->components).getStream()));
}

}
}
