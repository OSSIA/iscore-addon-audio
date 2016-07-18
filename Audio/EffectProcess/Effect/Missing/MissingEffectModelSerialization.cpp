#include "MissingEffectModel.hpp"

template<>
void Visitor<Reader<DataStream>>::readFrom_impl(
        const Audio::Effect::MissingEffectModel& eff)
{
    ISCORE_TODO;
}

template<>
void Visitor<Writer<DataStream>>::writeTo(
        Audio::Effect::MissingEffectModel& eff)
{
    ISCORE_TODO;
}

template<>
void Visitor<Reader<JSONObject>>::readFrom_impl(
        const Audio::Effect::MissingEffectModel& area)
{
    ISCORE_TODO;
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(
        Audio::Effect::MissingEffectModel& area)
{
    ISCORE_TODO;
}
