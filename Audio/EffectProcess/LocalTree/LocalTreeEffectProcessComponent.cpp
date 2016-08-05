#include "LocalTreeEffectProcessComponent.hpp"
#include <OSSIA/LocalTree/Scenario/MetadataParameters.hpp>
#include <Audio/EffectProcess/EffectProcessModel.hpp>
#include <iscore/tools/SettableIdentifierGeneration.hpp>

namespace Audio
{
namespace Effect
{
namespace LocalTree
{
///////// Process component
EffectProcessComponentBase::EffectProcessComponentBase(
        const Id<iscore::Component>& id,
        ossia::net::node_base& parent,
        Effect::ProcessModel& scenario,
        Ossia::LocalTree::DocumentPlugin& doc,
        QObject* parent_obj):
    Ossia::LocalTree::ProcessComponent_T<ProcessModel>{parent, scenario, doc, id, "EffectProcessComponent", parent_obj},
    m_effectsNode{add_node(*node(), "effects")}
{
}

EffectComponent*EffectProcessComponentBase::make(
        const Id<iscore::Component>& id,
        EffectComponentFactory& factory,
        EffectModel& model)
{
    return factory.make(id, *m_effectsNode, model, this->system(), this);
}

void EffectProcessComponentBase::removing(const EffectModel& cst, const EffectComponent& comp)
{
    auto it = find_if(m_effectsNode->children(), [&] (const auto& node)
    { return node == comp.node(); });
    ISCORE_ASSERT(it != m_effectsNode->children().end());

    m_effectsNode->erase(it);
}

EffectProcessComponentBase::~EffectProcessComponentBase()
{
    m_properties.clear();

    m_effectsNode.reset();

    m_thisNode.clear();
}
}
}
}
