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

}
}
