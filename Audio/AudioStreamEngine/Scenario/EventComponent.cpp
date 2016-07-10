#include "EventComponent.hpp"

namespace Audio
{
namespace AudioStreamEngine
{
Event::Event(
        const Id<iscore::Component>& id,
        Scenario::EventModel& ev,
        const DocumentPlugin& doc,
        QObject* parent_comp):
    Component{id, "EventComponent", parent_comp},
    event{ev}
{
}

}
}
