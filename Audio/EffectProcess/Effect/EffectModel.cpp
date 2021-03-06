#include "EffectModel.hpp"
#include <Audio/EffectProcess/Effect/EffectParameters.hpp>
namespace Audio
{
namespace Effect
{
EffectModel::EffectModel(
        const Id<EffectModel>& id,
        QObject* parent):
    Entity{id, staticMetaObject.className(), parent}
{
    metadata().setInstanceName(*this);
}

EffectModel::EffectModel(
        const EffectModel& source,
        const Id<EffectModel>& id,
        QObject* parent):
    Entity{source, id, staticMetaObject.className(), parent},
    m_params{source.getParams()}
{
  metadata().setInstanceName(*this);
}

EffectModel::~EffectModel()
{

}

std::vector<float> EffectModel::getParams() const
{
    std::vector<float> p;
    if(m_effect)
    {
        int n = GetControlCountEffect(m_effect);
        p.reserve(n);
        for(int i = 0; i < n; i++)
        {
            p.push_back(GetControlValueEffect(m_effect, i));
        }
    }
    return p;

}

void EffectModel::saveParams() const
{
    m_params = getParams();
}

void EffectModel::restoreParams()
{
    if(!m_effect)
        return;

    std::size_t n = GetControlCountEffect(m_effect);
    if(m_params.size() != n)
        return;

    for(int i = 0; i < n; i++)
        SetControlValueEffect(m_effect, i, m_params[i]);

}

}
}
