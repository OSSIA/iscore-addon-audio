#include "SoundProcessModel.hpp"
#include <iscore/plugins/documentdelegate/plugin/ElementPluginModelList.hpp>

template<>
void Visitor<Reader<DataStream>>::readFrom_impl(const Audio::Sound::ProcessModel& proc)
{
    readFrom(*proc.pluginModelList);

    m_stream << proc.m_file;

    insertDelimiter();
}

template<>
void Visitor<Writer<DataStream>>::writeTo(Audio::Sound::ProcessModel& proc)
{
    proc.pluginModelList = new iscore::ElementPluginModelList{*this, &proc};

    m_stream >> proc.m_file;

    checkDelimiter();
}

template<>
void Visitor<Reader<JSONObject>>::readFrom_impl(const Audio::Sound::ProcessModel& proc)
{
    m_obj["PluginsMetadata"] = toJsonValue(*proc.pluginModelList);
    m_obj["File"] = toJsonObject(proc.file());
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(Audio::Sound::ProcessModel& proc)
{
    Deserializer<JSONValue> elementPluginDeserializer(m_obj["PluginsMetadata"]);
    proc.pluginModelList = new iscore::ElementPluginModelList{elementPluginDeserializer, &proc};

    proc.m_file = fromJsonObject<Audio::MediaFileHandle>(m_obj["File"]);
}
