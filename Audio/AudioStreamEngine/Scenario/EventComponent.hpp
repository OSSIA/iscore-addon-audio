#pragma once
#include <Scenario/Document/Event/EventModel.hpp>
#include <Network/Node.h>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>

namespace Audio
{
namespace AudioStreamEngine
{
class EventComponent final :
        public iscore::Component
{

    public:
        using system_t = Audio::AudioStreamEngine::DocumentPlugin;

        EventComponent(
                const Id<Component>& id,
                Scenario::EventModel& event,
                const system_t& doc,
                QObject* parent_comp);

        const Key& key() const override;

        ~EventComponent();

        std::function<void(audio_frame_t, bool)> onDateFixed;
        const Scenario::EventModel& event;
};
}
}
