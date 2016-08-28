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
    m_effectOrder.insert(pos, eff->id());

    emit effectsChanged();
}

void ProcessModel::removeEffect(const Id<EffectModel>& e)
{
    m_effectOrder.removeAll(e);

    m_effects.remove(e);

    emit effectsChanged();
}

void ProcessModel::moveEffect(const Id<EffectModel>& e, int new_pos)
{
    new_pos = clamp(new_pos, 0, m_effectOrder.size() - 1);
    auto old_pos = m_effectOrder.indexOf(e);
    if(old_pos != -1)
    {
        m_effectOrder.move(old_pos, new_pos);
        emit effectsChanged();
    }
}

int ProcessModel::effectPosition(const Id<EffectModel>& e) const
{
    return m_effectOrder.indexOf(e);
}

}

}
