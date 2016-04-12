#include "EffectModel.hpp"

template<>
void Visitor<Reader<DataStream>>::readFrom_impl(
        const Audio::Effect::EffectModel& eff)
{
    // Save the parent class
    readFrom(static_cast<const IdentifiedObject<Audio::Effect::EffectModel>&>(eff));

    // Save this class
    // ... nothing for now
}

template<>
void Visitor<Writer<DataStream>>::writeTo(
        Audio::Effect::EffectModel& eff)
{
}

template<>
void Visitor<Reader<JSONObject>>::readFrom_impl(
        const Audio::Effect::EffectModel& area)
{
    // Save the parent class
    readFrom(static_cast<const IdentifiedObject<Audio::Effect::EffectModel>&>(area));

}

template<>
void Visitor<Writer<JSONObject>>::writeTo(
        Audio::Effect::EffectModel& area)
{
}
