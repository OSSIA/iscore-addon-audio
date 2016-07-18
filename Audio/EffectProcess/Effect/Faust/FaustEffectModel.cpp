#include "FaustEffectModel.hpp"
#include <Audio/AudioStreamEngine/AudioApplicationPlugin.hpp>
namespace Audio
{
namespace Effect
{

FaustEffectModel::FaustEffectModel(
        const QString& faustProgram,
        const Id<EffectModel>& id,
        QObject* parent):
    EffectModel{id, parent}
{
    setText(faustProgram);
    init();
}

FaustEffectModel::FaustEffectModel(
        const FaustEffectModel& source,
        const Id<EffectModel>& id,
        QObject* parent):
    EffectModel{id, parent}
{
    setText(source.text());
    init();
}

FaustEffectModel*FaustEffectModel::clone(const Id<EffectModel>& newId, QObject* parent) const
{
    return new FaustEffectModel{*this, newId, parent};
}

QString FaustEffectModel::title() const
{
    if(m_name.isEmpty())
    {
        return tr("Faust");
    }
    else
    {
        return m_name;
    }
}

void FaustEffectModel::setText(const QString& txt)
{
    m_text = txt;
    reload();
    emit textChanged();
}

void FaustEffectModel::serialize_impl(const VisitorVariant& vis) const
{
    serialize_dyn(vis, *this);
}

void FaustEffectModel::init()
{
    // We have to reload the faust FX whenever
    // some soundcard settings changes
    auto& ctx = iscore::AppContext().components.applicationPlugin<Audio::AudioStreamEngine::ApplicationPlugin>();
    con(ctx, &AudioStreamEngine::ApplicationPlugin::audioEngineRestarted,
            this, [this] () {
        reload();
    });
}

void FaustEffectModel::reload()
{
    auto fx_text = m_text.toLocal8Bit();
    if(!fx_text.isEmpty())
        m_effect = MakeFaustAudioEffect(fx_text, "/usr/local/lib/faust/", ""); // TODO compute the path to the "architecture" folder here

    if(m_effect)
    {
        auto json = GetJsonEffect(m_effect);
        QJsonParseError err;
        auto qjs = QJsonDocument::fromJson(json, &err);
        if(err.error == QJsonParseError::NoError)
        {
            m_name = qjs.object()["name"].toString();
        }
        else
        {
            qDebug() << err.errorString();
        }
    }
    else
    {
        qDebug() << "could not load effect";
    }
}

}
}
