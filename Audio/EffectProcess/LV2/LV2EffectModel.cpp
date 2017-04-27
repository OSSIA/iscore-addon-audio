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

    auto& app_plug = iscore::AppComponents().guiApplicationPlugin<Audio::AudioStreamEngine::ApplicationPlugin>();
    auto& world = app_plug.lilv;

    auto plugs = world.get_all_plugins();
    auto it = plugs.begin();
    while(!plugs.is_end(it))
    {
        auto plug = plugs.get(it);
        if((isFile && QString(plug.get_bundle_uri().as_string()) == path) || (!isFile && QString(plug.get_name().as_string()) == path))
        {
            plugin = plug.me;
            metadata().setLabel(QString(plug.get_name().as_string()));
            effectContext.plugin.me = plug;
            m_effect = MakeLV2AudioEffect(&app_plug.lv2_host_context, &effectContext);
            return;
        }
        else if(!isFile && QString(plug.get_name().as_string()) == path)
        {
            plugin = plug.me;
            effectContext.plugin.me = plug;
            m_effect = MakeLV2AudioEffect(&app_plug.lv2_host_context, &effectContext);
            metadata().setLabel(QString(plug.get_name().as_string()));
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

template <>
void DataStreamReader::read(
        const Audio::Effect::LV2EffectModel& eff)
{
    m_stream << eff.effect();
    insertDelimiter();
}

template <>
void DataStreamWriter::write(
        Audio::Effect::LV2EffectModel& eff)
{
    m_stream >> eff.m_effectPath;
    checkDelimiter();
}

template <>
void JSONObjectReader::read(
        const Audio::Effect::LV2EffectModel& eff)
{
    obj["Effect"] = eff.effect();
}

template <>
void JSONObjectWriter::write(
        Audio::Effect::LV2EffectModel& eff)
{
    eff.m_effectPath = obj["Effect"].toString();
}
