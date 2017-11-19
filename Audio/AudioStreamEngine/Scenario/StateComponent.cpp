#include "StateComponent.hpp"

namespace Audio
{
namespace AudioStreamEngine
{

State::State(
        const Id<score::Component>& id,
        Scenario::StateModel& state,
        const State::system_t& doc,
        QObject* parent_comp):
    Component{id, "StateComponent", parent_comp}
{
}
}
}
