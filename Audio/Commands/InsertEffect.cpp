#include "InsertEffect.hpp"

#include <iscore/tools/SettableIdentifierGeneration.hpp>
#include <iscore/tools/ModelPathSerialization.hpp>
#include <Audio/EffectProcess/EffectProcessModel.hpp>
#include <Audio/EffectProcess/Effect/EffectFactory.hpp>
namespace Audio
{
namespace Commands
{

InsertEffect::InsertEffect(
        const Effect::ProcessModel& model,
        const UuidKey<Effect::EffectFactory>& effectKind,
        const QString& text,
        int effectPos):
    m_model{model},
    m_id{getStrongId(model.effects())},
    m_effectKind{effectKind},
    m_effect{text},
    m_pos{effectPos}
{
}

void InsertEffect::undo() const
{
    auto& process = m_model.find();
    auto& eff = process.effects().at(m_id);
    process.removeEffect(eff);
}

void InsertEffect::redo() const
{
    auto& process = m_model.find();
    auto& fact_list = context.components.factory<Effect::EffectFactoryList>();

    if(Effect::EffectFactory* fact = fact_list.get(m_effectKind))
    {
        auto model = fact->makeModel(m_effect, m_id, &process);
        process.insertEffect(model, m_pos);
    }
    else
    {
        ISCORE_TODO;
        // Insert a fake effect ?
    }
}

void InsertEffect::serializeImpl(DataStreamInput& s) const
{
    s << m_model << m_id << m_effectKind << m_effect << m_pos;
}

void InsertEffect::deserializeImpl(DataStreamOutput& s)
{
    s >> m_model >> m_id >> m_effectKind >> m_effect >> m_pos;
}

}
}
