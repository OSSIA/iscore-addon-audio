#include "LocalTreeEffectComponent.hpp"

namespace Audio
{
namespace Effect
{
namespace LocalTree
{
///////// Process component factory
EffectComponent::EffectComponent(
        ossia::net::node_base& n,
        EffectModel& model,
        Ossia::LocalTree::DocumentPlugin& doc,
        const Id<iscore::Component>& id,
        const QString& name,
        QObject* parent):
    parent_t{n, model.metadata, model, doc, id, "EffectComponent", parent},
    m_parametersNode{*node().createChild("parameters")}
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
