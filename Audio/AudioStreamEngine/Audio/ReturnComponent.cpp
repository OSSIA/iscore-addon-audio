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
        Return::ProcessModel& sound,
        DocumentPlugin& doc,
        const Id<iscore::Component>& id,
        QObject* parent_obj):
    ProcessComponent_T{sound, doc, id, "ReturnComponent", parent_obj}
{

}

AudioGraphVertice ReturnComponent::visit(AudioGraph& graph)
{
    auto send = process().send_ptr();
    if(send)
    {
        return ProcessComponent::visit(graph);
    }
    return {};
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
