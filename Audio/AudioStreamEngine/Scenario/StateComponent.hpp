#pragma once
#include <Scenario/Document/State/StateModel.hpp>
#include <ossia/network/base/node.hpp>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>

namespace Audio
{
namespace AudioStreamEngine
{
class State final :
        public score::Component
{
        COMMON_COMPONENT_METADATA("1d598747-106d-431f-a465-bb47c70d03ab")
    public:
        using system_t = Audio::AudioStreamEngine::DocumentPlugin;

        State(
                const Id<score::Component>& id,
                Scenario::StateModel& state,
                const system_t& doc,
                QObject* parent_comp);

};
}
}
