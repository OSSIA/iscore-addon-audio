#include "LocalTreeEffectComponent.hpp"


namespace Audio
{
namespace Effect
{
namespace LocalTree
{
///////// Process componnet factory
EffectComponent::EffectComponent(
        OSSIA::Node& node,
        EffectModel& proc,
        Ossia::LocalTree::DocumentPlugin& doc,
        const Id<iscore::Component>& id,
        const QString& name,
        QObject* parent):
    GenericEffectComponent<Ossia::LocalTree::DocumentPlugin>{proc, doc, id, "EffectComponent", parent}
{
    m_thisNode = *node.emplace(node.children().end(), name.toStdString());
    m_parametersNode = *m_thisNode->emplace(m_thisNode->children().end(), "parameters");
}

EffectComponent::~EffectComponent()
{
    m_parametersNode.reset();
    m_thisNode.reset();
}

EffectComponentFactory::~EffectComponentFactory()
{

}
}
}
}
