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
        COMPONENT_METADATA("1d598747-106d-431f-a465-bb47c70d03ab")
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
