#include "EffectProcessModel.hpp"

template <>
void DataStreamReader::read(const Audio::Effect::ProcessModel& proc)
{
    int32_t n = proc.effects().size();
    m_stream << n;
    for(auto& eff : proc.effects())
        readFrom(eff);

    m_stream << proc.effectsOrder();
    insertDelimiter();
}

template <>
void DataStreamWriter::write(Audio::Effect::ProcessModel& proc)
{
    int32_t n = 0;
    m_stream >> n;

    auto& fxs = components.interfaces<Audio::Effect::EffectFactoryList>();
    for(int i = 0; i < n ; i++)
    {
        auto fx = deserialize_interface(fxs, *this, &proc);
        if(fx)
            proc.insertEffect(fx, i++);
        else
            ISCORE_TODO;
    }

    proc.m_effectOrder.clear();
    m_stream >> proc.m_effectOrder;

    checkDelimiter();
}

template <>
void JSONObjectReader::read(const Audio::Effect::ProcessModel& proc)
{
    obj["Effects"] = toJsonArray(proc.effects());
    obj["Order"] = toJsonArray(proc.effectsOrder());
}

template <>
void JSONObjectWriter::write(Audio::Effect::ProcessModel& proc)
{
    QJsonArray fx_array = obj["Effects"].toArray();
    auto& fxs = components.interfaces<Audio::Effect::EffectFactoryList>();
    int i = 0;
    for(const auto& json_vref : fx_array)
    {
        JSONObject::Deserializer deserializer{json_vref.toObject()};
        auto fx = deserialize_interface(fxs, deserializer, &proc);
        if(fx)
            proc.insertEffect(fx, i++);
        else
            ISCORE_TODO;
    }

    proc.m_effectOrder.clear();
    fromJsonValueArray(obj["Order"].toArray(), proc.m_effectOrder);
}
