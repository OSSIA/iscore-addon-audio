#pragma once
#include <OSSIA/LocalTree/LocalTreeDocumentPlugin.hpp>
#include <OSSIA/LocalTree/NameProperty.hpp>
#include <Audio/EffectProcess/EffectModel.hpp>
#include <Audio/AudioStreamEngine/Audio/EffectComponent.hpp>

namespace Audio
{
namespace Effect
{

template<typename EffectBase_T, typename Component_T>
class EffectComponentBase :
        public Component_T
{
    public:
        template<typename... Args>
        EffectComponentBase(EffectBase_T& cst, Args&&... args):
            Component_T{std::forward<Args>(args)...},
            m_effect{cst}
        {

        }

        EffectBase_T& effect() const
        { return m_effect; }

    private:
        EffectBase_T& m_effect;
};

template<typename Component_T>
using EffectComponent = EffectComponentBase<Effect::EffectModel, Component_T>;

template<typename System_T>
using GenericEffectComponent =
    Effect::EffectComponent<iscore::GenericComponent<System_T>>;



template<typename EffectComponentBase_T, typename Effect_T>
class GenericEffectComponent_T : public EffectComponentBase_T
{
    public:
        using model_type = Effect_T;
        using EffectComponentBase_T::EffectComponentBase_T;

        const Effect_T& effect() const
        { return static_cast<const Effect_T&>(EffectComponentBase_T::effect()); }
};


namespace LocalTree
{
class EffectComponent :
        public GenericEffectComponent<Ossia::LocalTree::DocumentPlugin>
{
    public:
        static const constexpr bool is_unique = true;
        EffectComponent(
                OSSIA::Node& node,
                Effect::EffectModel& proc,
                Ossia::LocalTree::DocumentPlugin& doc,
                const Id<iscore::Component>& id,
                const QString& name,
                QObject* parent);

        virtual ~EffectComponent();
        auto& node() const
        { return m_thisNode; }

    protected:
        std::shared_ptr<OSSIA::Node> m_parametersNode;

    private:
        std::shared_ptr<OSSIA::Node> m_thisNode;
        OSSIA::CallbackContainer<OSSIA::NodeChangeCallback>::iterator m_callbackIt;
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
