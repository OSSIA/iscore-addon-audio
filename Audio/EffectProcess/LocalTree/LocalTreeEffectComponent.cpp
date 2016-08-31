#include "LocalTreeEffectComponent.hpp"

#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <Audio/EffectProcess/Effect/EffectParameters.hpp>


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
    emit aboutToBeDestroyed();
}

void EffectComponent::recreate()
{
    m_parametersNode.clearChildren();

    AudioEffect fx = effect().effect();
    if(!fx)
        return;

    // TODO separate "address segment" and "name".
    for(EffectParameter parameter : AudioEffectParameterAdaptor{fx})
    {
        auto idx = parameter.label.lastIndexOf('/');
        if(idx != -1)
        {
            parameter.label = parameter.label.mid(idx);
        }

        auto str_label = parameter.label.toStdString();
        // Create the node
        auto param_node = m_parametersNode.createChild(str_label);
        auto param_addr = param_node->createAddress(ossia::val_type::FLOAT);
        param_addr->setAccessMode(ossia::access_mode::BI);
        param_addr->setDomain(ossia::net::make_domain(ossia::Float{parameter.min}, ossia::Float{parameter.max}));
        param_addr->setDescription(str_label);

        // Set value to current value of fx
        param_addr->add_callback([=,num=parameter.id] (const ossia::value& val) {
            if(val.getType() != ossia::val_type::FLOAT)
                return;
            auto fx = effect().effect();
            if(!fx)
                return;

            auto current_val = val.get<ossia::Float>().value;
            fx->SetControlValue(num, current_val);
        });
        param_addr->pushValue(ossia::Float{parameter.init});

    }
    emit effectTreeChanged();

}

}
}
}
