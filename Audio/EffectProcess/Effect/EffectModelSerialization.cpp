#include "EffectModel.hpp"

template<>
void Visitor<Reader<DataStream>>::readFrom_impl(
        const Audio::Effect::EffectModel& eff)
{
    // Save the parent class
    readFrom(static_cast<const iscore::Entity<Audio::Effect::EffectModel>&>(eff));

    eff.saveParams();
    m_stream << eff.m_params;
}

template<>
void Visitor<Writer<DataStream>>::writeTo(
        Audio::Effect::EffectModel& eff)
{
    m_stream >> eff.m_params;
}

template<>
void Visitor<Reader<JSONObject>>::readFrom_impl(
        const Audio::Effect::EffectModel& eff)
{
    // Save the parent class
    readFrom(static_cast<const iscore::Entity<Audio::Effect::EffectModel>&>(eff));

    eff.saveParams();
    m_obj["Parameters"] = toJsonValueArray(eff.m_params);
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(
        Audio::Effect::EffectModel& eff)
{
    fromJsonArray(m_obj["Parameters"].toArray(), eff.m_params);
}
