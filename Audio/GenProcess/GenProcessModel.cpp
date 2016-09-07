#include <Audio/GenProcess/GenProcessModel.hpp>
#include <Audio/AudioStreamEngine/AudioApplicationPlugin.hpp>
#include <QJsonDocument>

namespace Audio
{
namespace Gen
{

ProcessModel::ProcessModel(
        const TimeValue& duration,
        const Id<Process::ProcessModel>& id,
        QObject* parent):
    Process::ProcessModel{duration, id, Metadata<ObjectKey_k, ProcessModel>::get(), parent}
{
    metadata().setName(Metadata<PrettyName_k, ProcessModel>::get() + QString(".%1").arg(*this->id().val()));
    init();
}

ProcessModel::ProcessModel(
        const ProcessModel& source,
        const Id<Process::ProcessModel>& id,
        QObject* parent):
    Process::ProcessModel{
        source.duration(),
        id,
        Metadata<ObjectKey_k, ProcessModel>::get(),
        parent}
{
    init();
}

ProcessModel::~ProcessModel()
{

}

void ProcessModel::setText(const QString& txt)
{
    m_text = txt;
    reload();
    emit textChanged();
}


void ProcessModel::init()
{
    // We have to reload the faust FX whenever
    // some soundcard settings changes
    auto& ctx = iscore::AppContext().components.applicationPlugin<Audio::AudioStreamEngine::ApplicationPlugin>();
    con(ctx, &AudioStreamEngine::ApplicationPlugin::audioEngineRestarted,
            this, [this] () {
        reload();
    });
}

void ProcessModel::reload()
{
    auto fx_text = m_text.toLocal8Bit();
    if(!fx_text.isEmpty())
        m_effect = MakeFaustAudioEffect(fx_text, "", ""); // TODO compute the path to the "architecture" folder here

}
}

}
