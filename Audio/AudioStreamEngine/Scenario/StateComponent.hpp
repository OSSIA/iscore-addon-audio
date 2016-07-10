#pragma once
#include <Scenario/Document/State/StateModel.hpp>
#include <Network/Node.h>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>

namespace Audio
{
namespace AudioStreamEngine
{
class State final :
        public iscore::Component
{
    public:
        using system_t = Audio::AudioStreamEngine::DocumentPlugin;

        State(
                const Id<iscore::Component>& id,
                Scenario::StateModel& state,
                const system_t& doc,
                QObject* parent_comp);

};
}
}
