#include <Audio/SoundProcess/SoundProcessModel.hpp>
#include <Process/Dummy/DummyLayerModel.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>
#include <iscore/plugins/documentdelegate/plugin/ElementPluginModelList.hpp>

#include <QFile>
#include <Audio/SoundProcess/SoundProcessLayer.hpp>

namespace Audio
{
namespace Sound
{

ProcessModel::ProcessModel(
        const TimeValue& duration,
        const Id<Process::ProcessModel>& id,
        QObject* parent):
    Process::ProcessModel{duration, id, Metadata<ObjectKey_k, ProcessModel>::get(), parent}
{
    pluginModelList = new iscore::ElementPluginModelList{
                      iscore::IDocument::documentContext(*parent),
                      this};

    setFile("/tmp/bass.aif");
}

ProcessModel::ProcessModel(
        const ProcessModel& source,
        const Id<Process::ProcessModel>& id,
        QObject* parent):
    Process::ProcessModel{
        source.duration(),
        id,
        Metadata<ObjectKey_k, ProcessModel>::get(),
        parent},
    m_file{source.m_file}
{
    pluginModelList = new iscore::ElementPluginModelList{
                      *source.pluginModelList,
            this};
}

ProcessModel::~ProcessModel()
{

}

void ProcessModel::setFile(const QString &file)
{
    if(file != m_file.name())
    {
        m_file = MediaFileHandle(file);
        emit fileChanged();
    }
}

void ProcessModel::setFile(const MediaFileHandle &file)
{
    m_file = file;
    emit fileChanged();
}
}

}
