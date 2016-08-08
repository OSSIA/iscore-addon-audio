#pragma once
#include <Audio/EffectProcess/LocalTree/LocalTreeEffectComponent.hpp>
#include <Audio/EffectProcess/Effect/Faust/FaustEffectModel.hpp>
namespace Audio
{
namespace Effect
{
namespace LocalTree
{

class FaustComponent : public EffectComponent_T<Effect::FaustEffectModel>
{
        COMPONENT_METADATA("4309e626-e125-4c5c-9e4a-86f2a85cd0a1")

    public:
        FaustComponent(
                const Id<iscore::Component>& id,
                ossia::net::node_base& node,
                Effect::FaustEffectModel& proc,
                Engine::LocalTree::DocumentPlugin& doc,
                QObject* parent);

        void recreate();

        AudioEffect m_audio_effect{};
};

using FaustComponentFactory = EffectComponentFactory_T<FaustComponent>;
}
}
}
