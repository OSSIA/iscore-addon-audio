#include "EffectModel.hpp"

template <>
void DataStreamReader::read(
        const Audio::Effect::EffectModel& eff)
{
    eff.saveParams();
    m_stream << eff.m_params;
}

template <>
void DataStreamWriter::writeTo(
        Audio::Effect::EffectModel& eff)
{
    m_stream >> eff.m_params;
}

template <>
void JSONObjectReader::read(
        const Audio::Effect::EffectModel& eff)
{
    eff.saveParams();
    obj["Parameters"] = toJsonValueArray(eff.m_params);
}

template <>
void JSONObjectWriter::writeTo(
        Audio::Effect::EffectModel& eff)
{
    fromJsonArray(obj["Parameters"].toArray(), eff.m_params);
}
