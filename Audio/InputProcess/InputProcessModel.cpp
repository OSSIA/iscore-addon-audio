#include <Audio/InputProcess/InputProcessModel.hpp>

#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>
#include <Scenario/Document/Interval/IntervalModel.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <Media/Sound/SoundModel.hpp>
#include <Audio/EffectProcess/EffectProcessModel.hpp>
#include <Loop/LoopProcessModel.hpp>
#include <QFile>

namespace Audio
{
namespace Input
{

ProcessModel::ProcessModel(
        const TimeVal& duration,
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
        parent}
{
    metadata().setInstanceName(*this);
}

ProcessModel::~ProcessModel()
{

}

}

}
