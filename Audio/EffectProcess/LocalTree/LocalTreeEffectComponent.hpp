#pragma once
#include <OSSIA/LocalTree/LocalTreeComponent.hpp>
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
        public Ossia::LocalTree::Component<GenericEffectComponent<Ossia::LocalTree::DocumentPlugin>>
{
    public:
        using parent_t = Ossia::LocalTree::Component<GenericEffectComponent<Ossia::LocalTree::DocumentPlugin>>;
        static const constexpr bool is_unique = true;
        EffectComponent(
                OSSIA::Node& node,
                Effect::EffectModel& proc,
                Ossia::LocalTree::DocumentPlugin& doc,
                const Id<iscore::Component>& id,
                const QString& name,
                QObject* parent);

        virtual ~EffectComponent();

    protected:
        std::shared_ptr<OSSIA::Node> m_parametersNode;
};

template<typename Effect_T>
using EffectComponent_T = Effect::GenericEffectComponent_T<EffectComponent, Effect_T>;


class EffectComponentFactory :
        public iscore::GenericComponentFactory<
            Effect::EffectModel,
            Ossia::LocalTree::DocumentPlugin,
            Effect::LocalTree::EffectComponentFactory>
{
        ISCORE_ABSTRACT_FACTORY("c9e1f9bc-b974-4695-a3a8-f797c34858ee")
    public:
        virtual ~EffectComponentFactory();
        virtual EffectComponent* make(
                const Id<iscore::Component>&,
                OSSIA::Node& n,
                Effect::EffectModel& proc,
                Ossia::LocalTree::DocumentPlugin& doc,
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
                OSSIA::Node& n,
                Effect::EffectModel& proc,
                Ossia::LocalTree::DocumentPlugin& doc,
                QObject* paren_objt) const override
        {
            return new EffectComponent_T{id, n, static_cast<model_type&>(proc), doc, paren_objt};
        }
};

using EffectComponentFactoryList =
    iscore::GenericComponentFactoryList<
            Effect::EffectModel,
            Ossia::LocalTree::DocumentPlugin,
            Effect::LocalTree::EffectComponentFactory>;
}
}
}

