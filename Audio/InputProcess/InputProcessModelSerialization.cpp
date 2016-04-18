#include "InputProcessModel.hpp"
#include <iscore/plugins/documentdelegate/plugin/ElementPluginModelList.hpp>

template<>
void Visitor<Reader<DataStream>>::readFrom_impl(const Audio::Input::ProcessModel& proc)
{
    readFrom(*proc.pluginModelList);

    insertDelimiter();
}

template<>
void Visitor<Writer<DataStream>>::writeTo(Audio::Input::ProcessModel& proc)
{
    proc.pluginModelList = new iscore::ElementPluginModelList{*this, &proc};

    checkDelimiter();
}

template<>
void Visitor<Reader<JSONObject>>::readFrom_impl(const Audio::Input::ProcessModel& proc)
{
    m_obj["PluginsMetadata"] = toJsonValue(*proc.pluginModelList);
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(Audio::Input::ProcessModel& proc)
{
    Deserializer<JSONValue> elementPluginDeserializer(m_obj["PluginsMetadata"]);
    proc.pluginModelList = new iscore::ElementPluginModelList{elementPluginDeserializer, &proc};
}
