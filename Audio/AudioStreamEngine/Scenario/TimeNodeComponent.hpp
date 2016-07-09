#pragma once
#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>
#include <Network/Node.h>
#include <functional>
using audio_frame_t = int64_t;
namespace Audio
{
namespace AudioStreamEngine
{
class DocumentPlugin;
class TimeNode final :
        public iscore::Component
{
        Q_OBJECT
    public:
        using system_t = Audio::AudioStreamEngine::DocumentPlugin;

        TimeNode(
                const Id<iscore::Component>& id,
                Scenario::TimeNodeModel& timeNode,
                const system_t& doc,
                QObject* parent_comp);

        const Key& key() const override;

        ~TimeNode();


        std::function<void(audio_frame_t, bool)> onDateFixed;
        const Scenario::TimeNodeModel& timeNode;
};
}
}
