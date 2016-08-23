#include "EffectModel.hpp"

template<>
void Visitor<Reader<DataStream>>::readFrom_impl(
        const Audio::Effect::EffectModel& eff)
{
    // Save the parent class
    readFrom(static_cast<const IdentifiedObject<Audio::Effect::EffectModel>&>(eff));

    readFrom(eff.metadata);
}

template<>
void Visitor<Writer<DataStream>>::writeTo(
        Audio::Effect::EffectModel& eff)
{
    writeTo(eff.metadata);
}

template<>
void Visitor<Reader<JSONObject>>::readFrom_impl(
        const Audio::Effect::EffectModel& eff)
{
    // Save the parent class
    readFrom(static_cast<const IdentifiedObject<Audio::Effect::EffectModel>&>(eff));
    m_obj[strings.Metadata] = toJsonObject(eff.metadata);

}

template<>
void Visitor<Writer<JSONObject>>::writeTo(
        Audio::Effect::EffectModel& eff)
{
    eff.metadata = fromJsonObject<ModelMetadata>(m_obj[strings.Metadata]);
}
