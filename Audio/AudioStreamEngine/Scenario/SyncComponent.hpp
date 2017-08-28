#pragma once
#include <Scenario/Document/TimeSync/TimeSyncModel.hpp>
#include <ossia/network/base/node.hpp>
#include <functional>
using audio_frame_t = int64_t;
namespace Audio
{
namespace AudioStreamEngine
{
class DocumentPlugin;
class Sync final :
        public iscore::Component
{
        COMMON_COMPONENT_METADATA("76c10344-74ed-44fb-8e13-cf7907759e1a")
    public:
        using system_t = Audio::AudioStreamEngine::DocumentPlugin;

        Sync(
                const Id<iscore::Component>& id,
                Scenario::TimeSyncModel& timeSync,
                const system_t& doc,
                QObject* parent_comp);

        std::function<void(audio_frame_t, bool)> onDateFixed;
        const Scenario::TimeSyncModel& timeSync;
};
}
}
