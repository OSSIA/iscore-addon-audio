#include "LocalTreeEffectComponent.hpp"

#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <Audio/EffectProcess/Effect/EffectParameters.hpp>
#include <Audio/EffectProcess/LV2/LV2EffectModel.hpp>


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
    parent_t{n, model.metadata(), model, doc, id, "EffectComponent", parent},
    m_parametersNode{*node().createChild("parameters")}
{
    con(model, &EffectModel::effectChanged,
        this, &EffectComponent::recreate);

    recreate();
}

EffectComponent::~EffectComponent()
{
    for(const auto& tpl : m_inAddresses)
    {
        std::get<2>(tpl)->aboutToBeDeleted.disconnect<EffectComponent, &EffectComponent::on_nodeDeleted>(this);
    }
    for(const auto& tpl : m_outAddresses)
    {
        std::get<2>(tpl)->aboutToBeDeleted.disconnect<EffectComponent, &EffectComponent::on_nodeDeleted>(this);
    }
    emit aboutToBeDestroyed();
}

void EffectComponent::recreate()
{
    m_parametersNode.clearChildren();

    EffectModel& effect = this->effect();
    AudioEffect fx = effect.effect();
    if(!fx)
        return;

    // TODO separate "address segment" and "name".
    for(EffectParameter parameter : AudioEffectParameterAdaptor<InParameter>{fx})
    {
        auto idx = parameter.label.lastIndexOf('/');
        if(idx != -1)
        {
            parameter.label = parameter.label.mid(idx + 1);
        }

        auto str_label = parameter.label.toStdString();
        // Create the node
        auto param_node = m_parametersNode.createChild(str_label);

        param_node->aboutToBeDeleted.connect<EffectComponent, &EffectComponent::on_nodeDeleted>(this);
        auto param_addr = param_node->createAddress(ossia::val_type::FLOAT);
        param_addr->setAccessMode(ossia::access_mode::BI);
        param_addr->setDomain(ossia::make_domain(float{parameter.min}, float{parameter.max}));
        if(!str_label.empty())
          ossia::net::set_description(*param_node, str_label);
        else
          ossia::net::set_description(*param_node, ossia::none);

        // Set value to current value of fx
        param_addr->add_callback([=,num=parameter.id] (const ossia::value& val) {
            if(val.getType() != ossia::val_type::FLOAT)
                return;
            auto fx = this->effect().effect();
            if(!fx)
                return;

            auto current_val = val.get<float>();
            SetControlValueEffect(fx, num, current_val);
        });

        const auto& p = effect.savedParams();
        if(parameter.id < p.size())
            param_addr->pushValue(float{p[parameter.id]});
        else
            param_addr->pushValue(float{parameter.init});

        m_inAddresses.push_back(std::make_tuple(parameter.id, param_addr, param_node));

    }

    if(auto lv2_fx = dynamic_cast<LV2EffectModel*>(&effect))
    {
        for(EffectParameter parameter : AudioEffectParameterAdaptor<OutParameter>{fx})
        {
            auto idx = parameter.label.lastIndexOf('/');
            if(idx != -1)
            {
                parameter.label = parameter.label.mid(idx + 1);
            }

            auto str_label = parameter.label.toStdString();
            // Create the node
            auto param_node = m_parametersNode.createChild(str_label);
            param_node->aboutToBeDeleted.connect<EffectComponent, &EffectComponent::on_nodeDeleted>(this);
            auto param_addr = param_node->createAddress(ossia::val_type::FLOAT);
            param_addr->setAccessMode(ossia::access_mode::GET);
            param_addr->setDomain(ossia::make_domain(float{parameter.min}, float{parameter.max}));
            if(!str_label.empty())
              ossia::net::set_description(*param_node, str_label);
            else
              ossia::net::set_description(*param_node, ossia::none);

            param_addr->pushValue(float{GetLV2ControlOutValue(fx, parameter.id)});
            m_outAddresses.push_back(std::make_tuple(parameter.id, param_addr, param_node));
        }

        if(GetLV2ControlOutCount(fx) > 0)
        {
            lv2_fx->effectContext.on_outControlsChanged = [&] {
                auto fx = effect.effect();
                if(!fx)
                    return;

                for(auto p : m_outAddresses)
                {
                    std::get<1>(p)->pushValue(float{GetLV2ControlOutValue(fx, std::get<0>(p))});
                }
            };
        }
    }
    emit effectTreeChanged();

}

void EffectComponent::on_nodeDeleted(const ossia::net::node_base& n)
{
    auto finder = [&] (const auto& pair) {
        return std::get<2>(pair)== &n;
    };

    auto in_it = ossia::find_if(m_inAddresses, finder);

    if(in_it != m_inAddresses.end())
    {
        m_inAddresses.erase(in_it);
    }
    else
    {
        auto out_it = ossia::find_if(m_outAddresses, finder);

        if(out_it != m_outAddresses.end())
        {
            m_outAddresses.erase(out_it);
        }
    }
}

}
}
}
