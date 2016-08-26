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
        Q_OBJECT
        COMMON_COMPONENT_METADATA("c9e1f9bc-b974-4695-a3a8-f797c34858ee")
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

        void recreate();

        ossia::net::node_base& parameters() const
        { return m_parametersNode; }

    signals:
        void effectTreeChanged();

    protected:
        ossia::net::node_base& m_parametersNode;
};

}
}
}

