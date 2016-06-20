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
class TimeNodeComponent final :
        public iscore::Component
{
        Q_OBJECT
    public:
        using system_t = Audio::AudioStreamEngine::DocumentPlugin;

        TimeNodeComponent(
                const Id<iscore::Component>& id,
                Scenario::TimeNodeModel& timeNode,
                const system_t& doc,
                const iscore::DocumentContext& ctx,
                QObject* parent_comp);

        const Key& key() const override;

        ~TimeNodeComponent();


        std::function<void(audio_frame_t)> onDateFixed;
        const Scenario::TimeNodeModel& timeNode;
};
}
}
