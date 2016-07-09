#include "TimeNodeComponent.hpp"
#include <Scenario/Document/TimeNode/Trigger/TriggerModel.hpp>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>
namespace Audio
{
namespace AudioStreamEngine
{

TimeNode::TimeNode(
        const Id<iscore::Component>& id,
        Scenario::TimeNodeModel& tn,
        const TimeNode::system_t& doc,
        QObject* parent_comp):
    Component{id, "TimeNodeComponent", parent_comp},
    timeNode{tn}
{
}

const iscore::Component::Key&TimeNode::key() const
{
    static const Key k{"94e54e1c-5e33-4003-9b7d-fc69fb037e85"};
    return k;
}

TimeNode::~TimeNode()
{
}

}
}
