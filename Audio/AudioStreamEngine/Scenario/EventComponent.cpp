#include "EventComponent.hpp"

namespace Audio
{
namespace AudioStreamEngine
{
EventComponent::EventComponent(
        const Id<iscore::Component>& id,
        Scenario::EventModel& ev,
        const EventComponent::system_t& doc,
        QObject* parent_comp):
    Component{id, "EventComponent", parent_comp},
    event{ev}
{
}

const iscore::Component::Key&EventComponent::key() const
{
    static const Key k{"9ae8b1e7-f609-4d03-baf1-836f357b97f6"};
    return k;
}

EventComponent::~EventComponent()
{
}

}
}
