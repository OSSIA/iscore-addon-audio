#include <Audio/ReturnProcess/ReturnProcessModel.hpp>
#include <Process/Dummy/DummyLayerModel.hpp>
#include <iscore/plugins/documentdelegate/plugin/ElementPluginModelList.hpp>
#include <Audio/SendProcess/SendProcessModel.hpp>
#include <QFile>

namespace Audio
{
namespace Return
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

    metadata.setName(Metadata<PrettyName_k, ProcessModel>::get() + QString(".%1").arg(*this->id().val()));
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
    m_sendPath{source.m_sendPath}
{
    pluginModelList = new iscore::ElementPluginModelList{
                      *source.pluginModelList,
            this};
}

ProcessModel::~ProcessModel()
{

}

const Send::ProcessModel* ProcessModel::send_ptr() const
{
    auto maybe_send = m_sendPath.try_find();
    if(!maybe_send)
        return nullptr;

    return dynamic_cast<Send::ProcessModel*>(maybe_send);
}
}

}
