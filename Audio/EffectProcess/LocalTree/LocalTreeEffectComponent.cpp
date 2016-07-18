#include "LocalTreeEffectComponent.hpp"

namespace Audio
{
namespace Effect
{
namespace LocalTree
{
///////// Process component factory
EffectComponent::EffectComponent(
        OSSIA::Node& n,
        EffectModel& model,
        Ossia::LocalTree::DocumentPlugin& doc,
        const Id<iscore::Component>& id,
        const QString& name,
        QObject* parent):
    parent_t{n, model.metadata, model, doc, id, "EffectComponent", parent},
    m_parametersNode{add_node(*node(), "parameters")}
{
}

EffectComponent::~EffectComponent()
{
}

EffectComponentFactory::~EffectComponentFactory()
{

}
}
}
}
