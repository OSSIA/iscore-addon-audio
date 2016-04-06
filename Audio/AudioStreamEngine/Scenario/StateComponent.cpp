#include "StateComponent.hpp"

namespace Audio
{
namespace AudioStreamEngine
{

StateComponent::StateComponent(
        const Id<iscore::Component>& id,
        Scenario::StateModel& state,
        const StateComponent::system_t& doc,
        const iscore::DocumentContext& ctx,
        QObject* parent_comp):
    Component{id, "StateComponent", parent_comp}
{
}

const iscore::Component::Key&StateComponent::key() const
{
    static const Key k{"d5e98f7b-39cf-4ea2-a75a-8ef11b340c61"};
    return k;
}

StateComponent::~StateComponent()
{
}

}
}
