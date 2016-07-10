#pragma once
#include <Scenario/Document/Event/EventModel.hpp>
#include <Network/Node.h>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>

namespace Audio
{
namespace AudioStreamEngine
{
class Event final :
        public iscore::Component
{
        COMPONENT_METADATA("080226c0-b415-448d-977c-21d2996f63fd")
    public:
        Event(
                const Id<iscore::Component>& id,
                Scenario::EventModel& event,
                const DocumentPlugin& doc,
                QObject* parent_comp);

        std::function<void(audio_frame_t, bool)> onDateFixed;
        const Scenario::EventModel& event;
};
}
}
