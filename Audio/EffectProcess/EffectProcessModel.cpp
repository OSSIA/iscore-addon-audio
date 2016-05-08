#include <Audio/EffectProcess/EffectProcessModel.hpp>
#include <DummyProcess/DummyLayerModel.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>
#include <iscore/plugins/documentdelegate/plugin/ElementPluginModelList.hpp>

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
    pluginModelList = new iscore::ElementPluginModelList{
                      iscore::IDocument::documentContext(*parent),
                      this};
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
    pluginModelList = new iscore::ElementPluginModelList{
                      *source.pluginModelList,
            this};

    ISCORE_TODO;
}

ProcessModel::~ProcessModel()
{

}

void ProcessModel::insertEffect(
        EffectModel* eff,
        int pos)
{
    clamp(pos, 0, m_effectOrder.size());

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

ProcessModel* ProcessModel::clone(
        const Id<Process::ProcessModel>& newId,
        QObject* newParent) const
{
    return new ProcessModel{*this, newId, newParent};
}

QString ProcessModel::prettyName() const
{
    return tr("Effect Process");
}

QByteArray ProcessModel::makeLayerConstructionData() const
{
    return {};
}

void ProcessModel::setDurationAndScale(const TimeValue& newDuration)
{
    setDuration(newDuration);
}

void ProcessModel::setDurationAndGrow(const TimeValue& newDuration)
{
    setDuration(newDuration);
}

void ProcessModel::setDurationAndShrink(const TimeValue& newDuration)
{
    setDuration(newDuration);
}

void ProcessModel::startExecution()
{
}

void ProcessModel::stopExecution()
{
}

void ProcessModel::reset()
{
}

ProcessStateDataInterface* ProcessModel::startStateData() const
{
    return nullptr;
}

ProcessStateDataInterface* ProcessModel::endStateData() const
{
    return nullptr;
}

Selection ProcessModel::selectableChildren() const
{
    return {};
}

Selection ProcessModel::selectedChildren() const
{
    return {};
}

void ProcessModel::setSelection(const Selection&) const
{
}

void ProcessModel::serialize_impl(const VisitorVariant& s) const
{
    serialize_dyn(s, *this);
}

Process::LayerModel* ProcessModel::makeLayer_impl(
        const Id<Process::LayerModel>& viewModelId,
        const QByteArray& constructionData,
        QObject* parent)
{
    return new Dummy::DummyLayerModel{*this, viewModelId, parent};
}

Process::LayerModel* ProcessModel::loadLayer_impl(
        const VisitorVariant& vis,
        QObject* parent)
{
    return deserialize_dyn(vis, [&] (auto&& deserializer)
    {
        auto autom = new Dummy::DummyLayerModel{
                        deserializer, *this, parent};

        return autom;
    });
}

Process::LayerModel* ProcessModel::cloneLayer_impl(
        const Id<Process::LayerModel>& newId,
        const Process::LayerModel& source,
        QObject* parent)
{
    return new Dummy::DummyLayerModel{
        safe_cast<const Dummy::DummyLayerModel&>(source),
                *this,
                newId,
                parent};
}
}

}
