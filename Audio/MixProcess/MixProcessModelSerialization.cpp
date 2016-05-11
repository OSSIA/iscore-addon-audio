#include "MixProcessModel.hpp"
#include <iscore/plugins/documentdelegate/plugin/ElementPluginModelList.hpp>


template<>
void Visitor<Reader<DataStream>>::readFrom(const Audio::Mix::Routing& proc)
{
    m_stream << proc.in << proc.out << proc.mix << proc.enabled;

    insertDelimiter();
}

template<>
void Visitor<Writer<DataStream>>::writeTo(Audio::Mix::Routing& proc)
{
    m_stream >> proc.in >> proc.out >> proc.mix >> proc.enabled;

    checkDelimiter();
}

template<>
void Visitor<Reader<JSONObject>>::readFrom(const Audio::Mix::Routing& proc)
{
    m_obj["In"] = toJsonValue(proc.in);
    m_obj["Out"] = toJsonValue(proc.out);
    m_obj["Mix"] = proc.mix;
    m_obj["Enabled"] = proc.enabled;
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(Audio::Mix::Routing& proc)
{
    proc.in = fromJsonValue<Id<Process::ProcessModel>>(m_obj["In"]);
    proc.out = fromJsonValue<Id<Process::ProcessModel>>(m_obj["Out"]);
    proc.mix = m_obj["Mix"].toDouble();
    proc.enabled = m_obj["Enabled"].toBool();
}

template<>
void Visitor<Reader<DataStream>>::readFrom(const Audio::Mix::DirectMix& proc)
{
    m_stream << proc.process << proc.mix;

    insertDelimiter();
}

template<>
void Visitor<Writer<DataStream>>::writeTo(Audio::Mix::DirectMix& proc)
{
    m_stream >> proc.process >> proc.mix;

    checkDelimiter();
}

template<>
void Visitor<Reader<JSONObject>>::readFrom(const Audio::Mix::DirectMix& proc)
{
    m_obj["Process"] = toJsonValue(proc.process);
    m_obj["Mix"] = proc.mix;
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(Audio::Mix::DirectMix& proc)
{
    proc.process = fromJsonValue<Id<Process::ProcessModel>>(m_obj["Process"]);
    proc.mix = m_obj["Mix"].toDouble();
}



template<>
void Visitor<Reader<DataStream>>::readFrom_impl(const Audio::Mix::ProcessModel& proc)
{
    readFrom(*proc.pluginModelList);

    insertDelimiter();
}

template<>
void Visitor<Writer<DataStream>>::writeTo(Audio::Mix::ProcessModel& proc)
{
    proc.pluginModelList = new iscore::ElementPluginModelList{*this, &proc};

    checkDelimiter();
}

template<>
void Visitor<Reader<JSONObject>>::readFrom_impl(const Audio::Mix::ProcessModel& proc)
{
    m_obj["PluginsMetadata"] = toJsonValue(*proc.pluginModelList);
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(Audio::Mix::ProcessModel& proc)
{
    Deserializer<JSONValue> elementPluginDeserializer(m_obj["PluginsMetadata"]);
    proc.pluginModelList = new iscore::ElementPluginModelList{elementPluginDeserializer, &proc};
}
