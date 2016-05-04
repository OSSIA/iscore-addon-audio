#include "ReturnProcessModel.hpp"
#include <iscore/plugins/documentdelegate/plugin/ElementPluginModelList.hpp>
#include <iscore/tools/ModelPathSerialization.hpp>
#include <Audio/SendProcess/SendProcessModel.hpp>

template<>
void Visitor<Reader<DataStream>>::readFrom_impl(const Audio::Return::ProcessModel& proc)
{
    readFrom(*proc.pluginModelList);
    m_stream << proc.m_sendPath;
    insertDelimiter();
}

template<>
void Visitor<Writer<DataStream>>::writeTo(Audio::Return::ProcessModel& proc)
{
    proc.pluginModelList = new iscore::ElementPluginModelList{*this, &proc};
    m_stream >> proc.m_sendPath;
    checkDelimiter();
}

template<>
void Visitor<Reader<JSONObject>>::readFrom_impl(const Audio::Return::ProcessModel& proc)
{
    m_obj["PluginsMetadata"] = toJsonValue(*proc.pluginModelList);
    m_obj["Send"] = toJsonObject(proc.m_sendPath.unsafePath());
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(Audio::Return::ProcessModel& proc)
{
    Deserializer<JSONValue> elementPluginDeserializer(m_obj["PluginsMetadata"]);
    proc.pluginModelList = new iscore::ElementPluginModelList{elementPluginDeserializer, &proc};

    ObjectPath p = fromJsonObject<ObjectPath>(m_obj["Send"]);
    using path_t = Path<Audio::Send::ProcessModel>;
    proc.m_sendPath = path_t{std::move(p), path_t::UnsafeDynamicCreation{}};
}
