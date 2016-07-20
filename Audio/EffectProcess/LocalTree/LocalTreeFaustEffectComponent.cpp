#include "LocalTreeFaustEffectComponent.hpp"


namespace Audio
{
namespace Effect
{
namespace LocalTree
{
FaustComponent::FaustComponent(
        const Id<iscore::Component>& id,
        OSSIA::Node& node,
        FaustEffectModel& proc,
        Ossia::LocalTree::DocumentPlugin& doc,
        QObject* parent):
    EffectComponent_T<Effect::FaustEffectModel>{node, proc, doc, id, proc.metadata.name(), parent}
{
    // Create a fake effect on each change,
    // and check its number / range of parameters.

    // When execution starts, the audio execution sets its pointer
    // here.

    // When a parameter gets modified, the audio effect component is modified.

    con(proc, &FaustEffectModel::textChanged,
        this, &FaustComponent::recreate);
    recreate();
}

void FaustComponent::recreate()
{
    while(!m_parametersNode->children().empty())
    {
        m_parametersNode->erase(m_parametersNode->children().begin());
    }

    auto fx = effect().effect();
    if(!fx)
        return;

    auto count = GetControlCountEffect(fx);
    for(int i = 0; i < count; i++)
    {
        char label[64];
        float min, max, init;

        GetControlParamEffect(fx, i, label, &min, &max, &init);

        std::shared_ptr<OSSIA::Node> param_node = *m_parametersNode->emplace(
                                                      m_parametersNode->children().end(),
                                                      label,
                                                      OSSIA::Type::FLOAT,
                                                      OSSIA::AccessMode::BI,
                                                      OSSIA::Domain::create(OSSIA::Float{min}, OSSIA::Float{max}));

        // Set value to current value of fx
        auto addr = param_node->getAddress();
        addr->addCallback([=] (const OSSIA::SafeValue& val) {
            if(val.getType() != OSSIA::Type::FLOAT)
                return;
            if(!m_audio_effect)
                return;

            auto current_val = val.get<OSSIA::Float>().value;
            SetControlValueEffect(m_audio_effect, i, current_val);
        });
        addr->pushValue(OSSIA::Float{init});
    }
}
}
}
}
