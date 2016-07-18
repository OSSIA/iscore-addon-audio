#pragma once
#include <OSSIA/LocalTree/LocalTreeDocumentPlugin.hpp>
#include <OSSIA/LocalTree/NameProperty.hpp>
#include <Audio/EffectProcess/EffectModel.hpp>
#include <Audio/AudioStreamEngine/Audio/EffectComponent.hpp>

namespace Audio
{
namespace Effect
{
namespace LocalTree
{
class EffectComponent : public iscore::Component
{
    public:
        static const constexpr bool is_unique = true;
        EffectComponent(
                OSSIA::Node& node,
                Effect::EffectModel& proc,
                const Id<iscore::Component>& id,
                const QString& name,
                QObject* parent);

        virtual ~EffectComponent();
        auto& node() const
        { return m_thisNode; }

    protected:
        std::shared_ptr<OSSIA::Node> m_parametersNode;

    private:
        OSSIA::Node& thisNode() const
        { return *node(); }

        std::shared_ptr<OSSIA::Node> m_thisNode;
        OSSIA::CallbackContainer<OSSIA::NodeChangeCallback>::iterator m_callbackIt;
};


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
                OSSIA::Node& parent,
                Effect::EffectModel& proc,
                const Ossia::LocalTree::DocumentPlugin& doc,
                QObject* paren_objt) const = 0;
};

using EffectComponentFactoryList =
    iscore::GenericComponentFactoryList<
            Effect::EffectModel,
            Ossia::LocalTree::DocumentPlugin,
            Effect::LocalTree::EffectComponentFactory>;
}
}
}
