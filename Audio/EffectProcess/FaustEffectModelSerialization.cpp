#include "FaustEffectModel.hpp"

template<>
void Visitor<Reader<DataStream>>::readFrom_impl(
        const Audio::Effect::FaustEffectModel& eff)
{
    ISCORE_TODO;
}

template<>
void Visitor<Writer<DataStream>>::writeTo(
        Audio::Effect::FaustEffectModel& eff)
{
    ISCORE_TODO;
}

template<>
void Visitor<Reader<JSONObject>>::readFrom_impl(
        const Audio::Effect::FaustEffectModel& area)
{
    ISCORE_TODO;
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(
        Audio::Effect::FaustEffectModel& area)
{
    ISCORE_TODO;
}
