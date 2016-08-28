#include "LocalTreeEffectComponent.hpp"

#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <ossia/network/domain/domain.hpp>


namespace Audio
{
namespace Effect
{
namespace LocalTree
{
///////// Process component factory
EffectComponent::EffectComponent(
        ossia::net::node_base& n,
        EffectModel& model,
        Engine::LocalTree::DocumentPlugin& doc,
        const Id<iscore::Component>& id,
        const QString& name,
        QObject* parent):
    parent_t{n, model.metadata, model, doc, id, "EffectComponent", parent},
    m_parametersNode{*node().createChild("parameters")}
{
    con(model, &EffectModel::effectChanged,
        this, &EffectComponent::recreate);

    recreate();
}

EffectComponent::~EffectComponent()
{
}

void EffectComponent::recreate()
{
    m_parametersNode.clearChildren();

    AudioEffect fx = effect().effect();
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
        param_addr->setDomain(ossia::net::make_domain(ossia::Float{min}, ossia::Float{max}));

        // Set value to current value of fx
        param_addr->add_callback([=] (const ossia::value& val) {
            if(val.getType() != ossia::val_type::FLOAT)
                return;
            auto fx = effect().effect();
            if(!fx)
                return;

            auto current_val = val.get<ossia::Float>().value;
            SetControlValueEffect(fx, i, current_val);
        });
        param_addr->pushValue(ossia::Float{init});
    }
    emit effectTreeChanged();

}

}
}
}
