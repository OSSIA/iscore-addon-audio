#include "SyncComponent.hpp"
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>
namespace Audio
{
namespace AudioStreamEngine
{

Sync::Sync(
        const Id<score::Component>& id,
        Scenario::TimeSyncModel& tn,
        const Sync::system_t& doc,
        QObject* parent_comp):
    Component{id, "SyncComponent", parent_comp},
    timeSync{tn}
{
}

}
}
