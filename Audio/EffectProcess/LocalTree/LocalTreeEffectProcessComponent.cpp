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
//////// Hierarchy
EffectComponentHierarchyManager::EffectComponentHierarchyManager(
        EffectProcessComponent& component,
        ProcessModel& cst,
        const Ossia::LocalTree::DocumentPlugin& doc,
        QObject* component_as_parent):
    process{cst},
    m_component{component},
    m_componentFactory{doc.context().app.components.factory<EffectComponentFactoryList>()},
    m_system{doc},
    m_parentObject{component_as_parent}
{
    for(auto& fx : process.effects())
    {
        add(fx);
    }

    process.effects().mutable_added.connect<
            EffectComponentHierarchyManager,
            &EffectComponentHierarchyManager::add>(this);

    process.effects().removing.connect<
            EffectComponentHierarchyManager,
            &EffectComponentHierarchyManager::remove>(this);
}

void EffectComponentHierarchyManager::add(EffectModel& process)
{
    // Will return a factory for the given process if available
    if(auto factory = m_componentFactory.factory(process, m_system))
    {
        // The subclass should provide this function to construct
        // the correct component relative to this process.
        auto proc_comp = m_component.make_effectComponent(
                             getStrongId(process.components),
                             *factory, process, m_system, m_parentObject);
        if(proc_comp)
        {
            process.components.add(proc_comp);
            m_children.emplace_back(EffectPair{process, *proc_comp});
        }
    }
}

void EffectComponentHierarchyManager::remove(const EffectComponentHierarchyManager::EffectPair& pair)
{
    m_component.removing(pair.model, pair.component);
    pair.model.components.remove(pair.component);
}

void EffectComponentHierarchyManager::clear()
{
    for(EffectPair element : m_children)
    {
        remove(element);
    }
    m_children.clear();
}

EffectComponentHierarchyManager::~EffectComponentHierarchyManager()
{
    clear();
}

void EffectComponentHierarchyManager::remove(const Effect::EffectModel& process)
{
    auto it = find_if(m_children, [&] (auto pair) {
        return &pair.model == &process;
    });

    if(it != m_children.end())
    {
        remove(*it);
        m_children.erase(it);
    }
}



///////// Process component
EffectProcessComponent::EffectProcessComponent(
        const Id<iscore::Component>& id,
        OSSIA::Node& parent,
        Effect::ProcessModel& scenario,
        Ossia::LocalTree::DocumentPlugin& doc,
        QObject* parent_obj):
    Ossia::LocalTree::ProcessComponent_T<ProcessModel>{parent, scenario, doc, id, "EffectProcessComponent", parent_obj},
    m_effectsNode{add_node(*node(), "effects")},
    m_hierarchy{*this, scenario, doc, this}
{
    make_metadata_node(scenario.metadata, *node(), m_properties, this);
}

EffectComponent*EffectProcessComponent::make_effectComponent(
        const Id<iscore::Component>& id,
        EffectComponentFactory& factory,
        EffectModel& process,
        const Ossia::LocalTree::DocumentPlugin& system,
        QObject* parent_component)
{
    return factory.make(id, *m_effectsNode, process, system, parent_component);
}

void EffectProcessComponent::removing(const EffectModel& cst, const EffectComponent& comp)
{
    auto it = find_if(m_effectsNode->children(), [&] (const auto& node)
    { return node == comp.node(); });
    ISCORE_ASSERT(it != m_effectsNode->children().end());

    m_effectsNode->erase(it);
}

EffectProcessComponent::~EffectProcessComponent()
{
    m_hierarchy.clear();

    m_properties.clear();

    m_effectsNode.reset();

    m_thisNode.clear();
}



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

EffectComponent* FaustComponentFactory::make(
        const Id<iscore::Component>& id,
        OSSIA::Node& parent,
        EffectModel& proc,
        const Ossia::LocalTree::DocumentPlugin& doc,
        QObject* parent_obj) const
{
    return new FaustComponent{parent, *safe_cast<FaustEffectModel*>(&proc), id, parent_obj};
}

EffectComponentFactory::~EffectComponentFactory()
{

}

}
}
}
