#include "LV2EffectModel.hpp"

template<>
void Visitor<Reader<DataStream>>::readFrom_impl(
        const Audio::Effect::LV2EffectModel& eff)
{
    insertDelimiter();
}

template<>
void Visitor<Writer<DataStream>>::writeTo(
        Audio::Effect::LV2EffectModel& eff)
{
    checkDelimiter();
}

template<>
void Visitor<Reader<JSONObject>>::readFrom_impl(
        const Audio::Effect::LV2EffectModel& eff)
{
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(
        Audio::Effect::LV2EffectModel& eff)
{
}
