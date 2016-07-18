#pragma once
#include <Audio/EffectProcess/LocalTree/LocalTreeEffectComponent.hpp>
#include <Audio/EffectProcess/FaustEffectModel.hpp>
namespace Audio
{
namespace Effect
{
namespace LocalTree
{

class FaustComponent : public EffectComponent
{
        COMPONENT_METADATA("4309e626-e125-4c5c-9e4a-86f2a85cd0a1")

        const Effect::FaustEffectModel& m_proc;
    public:
        FaustComponent(
                OSSIA::Node& node,
                Effect::FaustEffectModel& proc,
                const Id<iscore::Component>& id,
                QObject* parent);

        void recreate();

        AudioEffect m_audio_effect{};
};

class FaustComponentFactory :
        public EffectComponentFactory
{
        ISCORE_CONCRETE_FACTORY("4e00eaab-d3f4-46ec-b4dc-bc53bf104064")

    public:
        EffectComponent* make(
                const Id<iscore::Component>&,
                OSSIA::Node& parent,
                Effect::EffectModel& proc,
                const Ossia::LocalTree::DocumentPlugin& doc,
                QObject* paren_objt) const override;

        bool matches(const EffectModel & m) const override
        {
            return dynamic_cast<const FaustEffectModel*>(&m);
        }
};
}
}
}
