#include "SendComponent.hpp"
#include <Audio/SendProcess/SendProcessModel.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Audio/AudioStreamEngine/Scenario/ConstraintComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/ScenarioComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/LoopComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/EffectComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/SoundComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/ReturnComponent.hpp>
#include <Audio/AudioStreamEngine/Streams/AudioStreamIScoreExtensions.h>
#include <Audio/AudioStreamEngine/Utility.hpp>
#include <Audio/MixProcess/MixProcessModel.hpp>

namespace Audio
{
namespace AudioStreamEngine
{

SendComponent::SendComponent(
        const Id<iscore::Component>& id,
        Send::ProcessModel& sound,
        const SendComponent::system_t& doc,
        QObject* parent_obj):
    ProcessComponent_T{sound, id, "SendComponent", parent_obj}
{

}

void SendComponent::makeStream(const Context& ctx)
{
    // For all "generative" streams in the parent constraint,
    // take their "send" streams, create returns, mix the returns,
    // put it on input of the effect, create a send, return the output
    auto parent_cst = safe_cast<Scenario::ConstraintModel*>(process().parent());
    ConstraintComponent& cst_comp = iscore::component<ConstraintComponent>(parent_cst->components);

    m_stream = MakeSend(cst_comp.makeInputMix(this->process().id()));
}

}
}
