#include "FaustEffectModel.hpp"

template <>
void DataStreamReader::read(
        const Audio::Effect::FaustEffectModel& eff)
{
    m_stream << eff.text();
    insertDelimiter();
}

template <>
void DataStreamWriter::write(
        Audio::Effect::FaustEffectModel& eff)
{
    QString txt;
    m_stream >> txt;
    eff.setText(txt);
    checkDelimiter();
}

template <>
void JSONObjectReader::read(
        const Audio::Effect::FaustEffectModel& eff)
{
    obj["Effect"] = eff.text();
}

template <>
void JSONObjectWriter::write(
        Audio::Effect::FaustEffectModel& eff)
{
    eff.setText(obj["Effect"].toString());
}
