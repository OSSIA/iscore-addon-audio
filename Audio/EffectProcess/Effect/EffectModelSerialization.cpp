#include "EffectModel.hpp"

template <>
void DataStreamReader::read(
        const Audio::Effect::EffectModel& eff)
{
    eff.saveParams();
    m_stream << eff.m_params;
}

template <>
void DataStreamWriter::write(
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
void JSONObjectWriter::write(
        Audio::Effect::EffectModel& eff)
{
    fromJsonArray(obj["Parameters"].toArray(), eff.m_params);
}
