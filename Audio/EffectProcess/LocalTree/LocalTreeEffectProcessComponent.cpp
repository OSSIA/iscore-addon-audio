#include "LocalTreeEffectProcessComponent.hpp"
#include <Engine/LocalTree/Scenario/MetadataParameters.hpp>
#include <Audio/EffectProcess/EffectProcessModel.hpp>
#include <score/tools/IdentifierGeneration.hpp>
#include <Audio/EffectProcess/LocalTree/LocalTreeEffectComponent.hpp>
#include <ossia/editor/state/state_element.hpp>

namespace Audio
{
namespace Effect
{
namespace LocalTree
{
///////// Process component
EffectProcessComponentBase::EffectProcessComponentBase(
        const Id<score::Component>& id,
        ossia::net::node_base& parent,
        Effect::ProcessModel& scenario,
        Engine::LocalTree::DocumentPlugin& doc,
        QObject* parent_obj):
    Engine::LocalTree::ProcessComponent_T<ProcessModel>{parent, scenario, doc, id, "EffectProcessComponent", parent_obj},
    m_effectsNode{*node().create_child("effects")}
{
}

EffectComponent*EffectProcessComponentBase::make(
        const Id<score::Component>& id,
        EffectModel& model)
{
    return new EffectComponent{m_effectsNode, model, this->system(), id, "EffectComponent", this};
}

void EffectProcessComponentBase::removing(const EffectModel& cst, const EffectComponent& comp)
{
}

EffectProcessComponentBase::~EffectProcessComponentBase()
{
}
}
}
}
