#include <Audio/EffectProcess/EffectProcessModel.hpp>
#include <Process/Dummy/DummyLayerModel.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>

#include <iscore/tools/Clamp.hpp>
#include <QFile>

namespace Audio
{
namespace Effect
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
    ISCORE_TODO;
}

ProcessModel::~ProcessModel()
{
    // TODO delete components

}

void ProcessModel::insertEffect(
        EffectModel* eff,
        int pos)
{
    clamp(pos, 0, int(m_effectOrder.size()));

    m_effects.add(eff);
    auto it = m_effectOrder.begin();
    std::advance(it, pos);
    m_effectOrder.insert(it, eff->id());

    emit effectsChanged();
}

void ProcessModel::removeEffect(const EffectModel& e)
{
    ISCORE_TODO;

    emit effectsChanged();
}

}

}
