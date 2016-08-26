#pragma once
#include <Engine/LocalTree/Scenario/ProcessComponent.hpp>
#include <Engine/LocalTree/LocalTreeComponent.hpp>
#include <Engine/LocalTree/NameProperty.hpp>
#include <Audio/EffectProcess/Effect/EffectModel.hpp>
#include <Audio/EffectProcess/EffectProcessModel.hpp>
#include <Audio/EffectProcess/Effect/Faust/FaustEffectModel.hpp>
#include <Audio/EffectProcess/LocalTree/LocalTreeEffectComponent.hpp>

#include <Audio/AudioStreamEngine/Audio/EffectComponent.hpp>
#include <iscore/tools/SettableIdentifierGeneration.hpp>
#include <iscore/component/ComponentHierarchy.hpp>

namespace Audio
{
namespace Effect
{
class ProcessModel;

namespace LocalTree
{
class EffectComponent;
class EffectProcessComponent;
class EffectFactoryList;

class EffectProcessComponentBase :
        public Engine::LocalTree::ProcessComponent_T<ProcessModel>
{
        COMPONENT_METADATA("0313e6df-1a18-4349-a4fb-8bc3461cc6b5")

    public:
            using model_t = EffectModel;
            using component_t = EffectComponent;

       EffectProcessComponentBase(
               const Id<iscore::Component>& id,
               ossia::net::node_base& parent,
               Effect::ProcessModel& scenario,
               Engine::LocalTree::DocumentPlugin& doc,
               QObject* parent_obj);


       EffectComponent* make(
               const Id<iscore::Component> & id,
               Effect::EffectModel &process);

       void removing(const Effect::EffectModel& cst, const EffectComponent& comp);

       ~EffectProcessComponentBase();


       template<typename Models>
       auto& models() const
       {
           static_assert(std::is_same<Models, EffectModel>::value,
                         "Effect component must be passed EffectModel as child.");

           return process().effects();
       }


    private:
        ossia::net::node_base& m_effectsNode;
};

class EffectProcessComponent final :
        public iscore::ComponentHierarchy<EffectProcessComponentBase>
{
    public:
        using iscore::ComponentHierarchy<EffectProcessComponentBase>::ComponentHierarchyManager;
};

using EffectProcessComponentFactory = Engine::LocalTree::ProcessComponentFactory_T<EffectProcessComponent>;
}
}
}
