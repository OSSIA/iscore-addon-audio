#pragma once
#include <Engine/LocalTree/LocalTreeComponent.hpp>
#include <Audio/EffectProcess/Effect/EffectModel.hpp>
#include <Audio/EffectProcess/Effect/EffectComponent.hpp>
#include <iscore/component/ComponentFactory.hpp>
#include <iscore/plugins/customfactory/ModelFactory.hpp>

namespace Audio
{
namespace Effect
{
namespace LocalTree
{
class EffectComponent :
        public Engine::LocalTree::Component<GenericEffectComponent<Engine::LocalTree::DocumentPlugin>>
{
        ABSTRACT_COMPONENT_METADATA(Audio::Effect::LocalTree::EffectComponent, "c9e1f9bc-b974-4695-a3a8-f797c34858ee")
    public:
        using parent_t = Engine::LocalTree::Component<GenericEffectComponent<Engine::LocalTree::DocumentPlugin>>;
        static const constexpr bool is_unique = true;
        EffectComponent(
                ossia::net::node_base& node,
                Effect::EffectModel& proc,
                Engine::LocalTree::DocumentPlugin& doc,
                const Id<iscore::Component>& id,
                const QString& name,
                QObject* parent);

        virtual ~EffectComponent();

        ossia::net::node_base& parameters() const
        { return m_parametersNode; }

    protected:
        ossia::net::node_base& m_parametersNode;
};

template<typename Effect_T>
using EffectComponent_T = Effect::GenericEffectComponent_T<EffectComponent, Effect_T>;


class EffectComponentFactory :
        public iscore::GenericComponentFactory<
            Effect::EffectModel,
            Engine::LocalTree::DocumentPlugin,
            Effect::LocalTree::EffectComponentFactory>
{
        ISCORE_ABSTRACT_COMPONENT_FACTORY(Audio::Effect::LocalTree::EffectComponent)
    public:
        virtual ~EffectComponentFactory();
        virtual EffectComponent* make(
                const Id<iscore::Component>&,
                ossia::net::node_base& n,
                Effect::EffectModel& proc,
                Engine::LocalTree::DocumentPlugin& doc,
                QObject* paren_objt) const = 0;
};


template<typename EffectComponent_T>
class EffectComponentFactory_T :
        public iscore::GenericComponentFactoryImpl<EffectComponent_T, EffectComponentFactory>
{
    public:
        using model_type = typename EffectComponent_T::model_type;
        EffectComponent* make(
                const Id<iscore::Component>& id,
                ossia::net::node_base& n,
                Effect::EffectModel& proc,
                Engine::LocalTree::DocumentPlugin& doc,
                QObject* paren_objt) const override
        {
            return new EffectComponent_T{id, n, static_cast<model_type&>(proc), doc, paren_objt};
        }
};

using EffectComponentFactoryList =
    iscore::GenericComponentFactoryList<
            Effect::EffectModel,
            Engine::LocalTree::DocumentPlugin,
            Effect::LocalTree::EffectComponentFactory>;
}
}
}

