#include "LV2EffectModel.hpp"
#include <Audio/AudioStreamEngine/AudioApplicationPlugin.hpp>
#include <Audio/AudioStreamEngine/Streams/AudioStreamIScoreExtensions.h>
#include <QUrl>
#include <QFile>

namespace Audio
{
namespace Effect
{

LV2EffectModel::LV2EffectModel(
        const QString& path,
        const Id<EffectModel>& id,
        QObject* parent):
    EffectModel{id, parent},
    m_effectPath{path}
{
    reload();
}

LV2EffectModel::LV2EffectModel(
        const LV2EffectModel& source,
        const Id<EffectModel>& id,
        QObject* parent):
    EffectModel{id, parent},
    m_effectPath{source.effect()}
{
    reload();
}

void LV2EffectModel::reload()
{
    plugin = nullptr;
    auto path = m_effectPath;
    if(path.isEmpty())
        return;

    bool isFile = QFile(QUrl(path).toString(QUrl::PreferLocalFile)).exists();
    if(isFile)
    {
        if(*path.rbegin() != '/')
            path.append('/');
    }

    auto& world = iscore::AppContext().components.applicationPlugin<Audio::AudioStreamEngine::ApplicationPlugin>().lilv;

    auto plugs = world.get_all_plugins();
    auto it = plugs.begin();
    while(!plugs.is_end(it))
    {
        auto plug = plugs.get(it);
        if((isFile && QString(plug.get_bundle_uri().as_string()) == path) || (!isFile && QString(plug.get_name().as_string()) == path))
        {
            plugin = plug.me;
            metadata().setName(QString(plug.get_name().as_string()));
            m_effect = MakeLV2AudioEffect(plug.me, world.me);
            return;
        }
        else if(!isFile && QString(plug.get_name().as_string()) == path)
        {
            plugin = plug.me;
            m_effect = MakeLV2AudioEffect(plug.me, world.me);
            metadata().setName(QString(plug.get_name().as_string()));
            return;
        }
        else
        {
            it = plugs.next(it);
        }
    }

    restoreParams();

}
}
}

template<>
void Visitor<Reader<DataStream>>::readFrom_impl(
        const Audio::Effect::LV2EffectModel& eff)
{
    m_stream << eff.effect();
    insertDelimiter();
}

template<>
void Visitor<Writer<DataStream>>::writeTo(
        Audio::Effect::LV2EffectModel& eff)
{
    m_stream >> eff.m_effectPath;
    checkDelimiter();
}

template<>
void Visitor<Reader<JSONObject>>::readFrom_impl(
        const Audio::Effect::LV2EffectModel& eff)
{
    m_obj["Effect"] = eff.effect();
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(
        Audio::Effect::LV2EffectModel& eff)
{
    eff.m_effectPath = m_obj["Effect"].toString();
}
