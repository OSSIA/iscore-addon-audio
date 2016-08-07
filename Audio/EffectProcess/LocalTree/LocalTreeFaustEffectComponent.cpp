#include "LocalTreeFaustEffectComponent.hpp"
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/erase.hpp>


namespace Audio
{
namespace Effect
{
namespace LocalTree
{
FaustComponent::FaustComponent(
        const Id<iscore::Component>& id,
        ossia::net::node_base& node,
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
    m_parametersNode.clearChildren();

    auto fx = effect().effect();
    if(!fx)
        return;

    auto count = GetControlCountEffect(fx);
    for(int i = 0; i < count; i++)
    {
        char label[512];
        float min, max, init;

        GetControlParamEffect(fx, i, label, &min, &max, &init);

        std::string label_s{label};
        auto idx = label_s.find_last_of('/');
        if(idx != std::string::npos)
        {
            label_s = label_s.substr(idx + 1, label_s.size());
        }
        auto param_node = m_parametersNode.createChild(label_s);
        auto param_addr = param_node->createAddress(ossia::val_type::FLOAT);
        param_addr->setAccessMode(ossia::access_mode::BI);
        param_addr->setDomain(ossia::net::makeDomain(ossia::Float{min}, ossia::Float{max}));

        // Set value to current value of fx
        param_addr->add_callback([=] (const ossia::value& val) {
            if(val.getType() != ossia::val_type::FLOAT)
                return;
            if(!m_audio_effect)
                return;

            auto current_val = val.get<ossia::Float>().value;
            SetControlValueEffect(m_audio_effect, i, current_val);
        });
        param_addr->pushValue(ossia::Float{init});
    }
}
}
}
}
