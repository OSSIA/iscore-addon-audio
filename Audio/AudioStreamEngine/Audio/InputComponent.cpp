#include "InputComponent.hpp"
#include <Audio/InputProcess/InputProcessModel.hpp>
#include <LibAudioStreamMC++.h>
#include <Audio/AudioStreamEngine/Streams/AudioStreamIScoreExtensions.h>
namespace Audio
{
namespace AudioStreamEngine
{

InputComponent::InputComponent(
        Input::ProcessModel& Input,
        DocumentPlugin& doc,
        const Id<iscore::Component>& id,
        QObject* parent_obj):
    ProcessComponent_T{Input, doc, id, "InputComponent", parent_obj}
{
    m_realTime = true;
}

void InputComponent::makeStream(const Context& ctx)
{
    const auto& inputs = process().inputs();
    if(inputs.inputs.size() == 0)
        return;

    auto in = MakeInputSound();
    ISCORE_ASSERT(in);
    auto sel = MakeSelectSound(in, (long*)inputs.inputs.data(), inputs.inputs.size());
    ISCORE_ASSERT(sel);
    m_stream = MakeSend(sel);
    ISCORE_ASSERT(m_stream);
}

}
}
