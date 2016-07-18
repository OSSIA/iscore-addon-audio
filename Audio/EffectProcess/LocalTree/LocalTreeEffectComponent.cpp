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
        const Id<iscore::Component>& id,
        const QString& name,
        QObject* parent):
    Component{id, "EffectComponent", parent}
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
