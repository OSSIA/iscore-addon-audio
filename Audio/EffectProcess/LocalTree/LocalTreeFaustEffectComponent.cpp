#include "LocalTreeFaustEffectComponent.hpp"


namespace Audio
{
namespace Effect
{
namespace LocalTree
{
FaustComponent::FaustComponent(OSSIA::Node& node, FaustEffectModel& proc, const Id<iscore::Component>& id, QObject* parent):
    EffectComponent{node, proc, id, proc.title(), parent},
    m_proc{proc}
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

    auto fx = m_proc.effect();
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
                                                      OSSIA::Value::Type::FLOAT,
                                                      OSSIA::AccessMode::BI,
                                                      OSSIA::Domain::create(new OSSIA::Float{min}, new OSSIA::Float{max}));

        // Set value to current value of fx
        auto addr = param_node->getAddress();
        addr->addCallback([=] (const OSSIA::Value& val) {
            if(val.getType() != OSSIA::Value::Type::FLOAT)
                return;
            if(!m_audio_effect)
                return;

            auto current_val = static_cast<const OSSIA::Float&>(val).value;
            SetControlValueEffect(m_audio_effect, i, current_val);
        });
        addr->pushValue(OSSIA::Float{init});
    }
}


EffectComponent* FaustComponentFactory::make(
        const Id<iscore::Component>& id,
        OSSIA::Node& parent,
        EffectModel& proc,
        const Ossia::LocalTree::DocumentPlugin& doc,
        QObject* parent_obj) const
{
    return new FaustComponent{parent, *safe_cast<FaustEffectModel*>(&proc), id, parent_obj};
}

}
}
}
