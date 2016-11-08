#include <Audio/ReturnProcess/ReturnProcessModel.hpp>

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
    metadata().setInstanceName(*this);
}

ProcessModel::ProcessModel(
        const ProcessModel& source,
        const Id<Process::ProcessModel>& id,
        QObject* parent):
    Process::ProcessModel{
        source,
        id,
        Metadata<ObjectKey_k, ProcessModel>::get(),
        parent},
    m_sendPath{source.m_sendPath}
{
    metadata().setInstanceName(*this);
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
