#include "FaustEffectModel.hpp"

template<>
void Visitor<Reader<DataStream>>::readFrom_impl(
        const Audio::Effect::FaustEffectModel& eff)
{
    m_stream << eff.text();
    insertDelimiter();
}

template<>
void Visitor<Writer<DataStream>>::writeTo(
        Audio::Effect::FaustEffectModel& eff)
{
    QString txt;
    m_stream >> txt;
    eff.setText(txt);
    checkDelimiter();
}

template<>
void Visitor<Reader<JSONObject>>::readFrom_impl(
        const Audio::Effect::FaustEffectModel& eff)
{
    m_obj["Effect"] = eff.text();
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(
        Audio::Effect::FaustEffectModel& eff)
{
    eff.setText(m_obj["Effect"].toString());
}
