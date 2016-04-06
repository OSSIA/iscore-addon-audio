#include "TimeNodeComponent.hpp"
#include <Scenario/Document/TimeNode/Trigger/TriggerModel.hpp>

namespace Audio
{
namespace AudioStreamEngine
{

TimeNodeComponent::TimeNodeComponent(
        const Id<iscore::Component>& id,
        Scenario::TimeNodeModel& timeNode,
        const TimeNodeComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent_comp):
    Component{id, "TimeNodeComponent", parent_comp}
{
}

const iscore::Component::Key&TimeNodeComponent::key() const
{
    static const Key k{"94e54e1c-5e33-4003-9b7d-fc69fb037e85"};
    return k;
}

TimeNodeComponent::~TimeNodeComponent()
{
}

}
}
