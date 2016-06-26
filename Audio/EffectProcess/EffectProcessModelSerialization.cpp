#include "EffectProcessModel.hpp"
#include <iscore/plugins/documentdelegate/plugin/ElementPluginModelList.hpp>

template<>
void Visitor<Reader<DataStream>>::readFrom_impl(const Audio::Effect::ProcessModel& proc)
{
    readFrom(*proc.pluginModelList);
    int32_t n = proc.effects().size();
    m_stream << n;
    for(auto& eff : proc.effects())
        readFrom(eff);

    insertDelimiter();
}

template<>
void Visitor<Writer<DataStream>>::writeTo(Audio::Effect::ProcessModel& proc)
{
    proc.pluginModelList = new iscore::ElementPluginModelList{*this, &proc};

    int32_t n = 0;
    m_stream >> n;

    auto& fxs = context.components.factory<Audio::Effect::EffectFactoryList>();
    for(int i = 0; i < n ; i++)
    {
        auto fx = deserialize_interface(fxs, *this, &proc);
        ISCORE_ASSERT(fx);
        proc.insertEffect(fx, i++);
    }

    checkDelimiter();
}

template<>
void Visitor<Reader<JSONObject>>::readFrom_impl(const Audio::Effect::ProcessModel& proc)
{
    m_obj["PluginsMetadata"] = toJsonValue(*proc.pluginModelList);
    m_obj["Effects"] = toJsonArray(proc.effects());
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(Audio::Effect::ProcessModel& proc)
{
    Deserializer<JSONValue> elementPluginDeserializer(m_obj["PluginsMetadata"]);
    proc.pluginModelList = new iscore::ElementPluginModelList{elementPluginDeserializer, &proc};


    QJsonArray fx_array = m_obj["Effects"].toArray();
    auto& fxs = context.components.factory<Audio::Effect::EffectFactoryList>();
    int i = 0;
    for(const auto& json_vref : fx_array)
    {
        Deserializer<JSONObject> deserializer{json_vref.toObject()};
        auto fx = deserialize_interface(fxs, deserializer, &proc);
        ISCORE_ASSERT(fx);
        proc.insertEffect(fx, i++);
    }

}
