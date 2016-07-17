#include <Audio/SendProcess/SendProcessModel.hpp>
#include <Process/Dummy/DummyLayerModel.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <Audio/SoundProcess/SoundProcessModel.hpp>
#include <Audio/EffectProcess/EffectProcessModel.hpp>
#include <Loop/LoopProcessModel.hpp>
#include <QFile>

namespace Audio
{
namespace Send
{

ProcessModel::ProcessModel(
        const TimeValue& duration,
        const Id<Process::ProcessModel>& id,
        QObject* parent):
    Process::ProcessModel{duration, id, Metadata<ObjectKey_k, ProcessModel>::get(), parent}
{
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
        parent}
{
}

ProcessModel::~ProcessModel()
{

}

}

}
