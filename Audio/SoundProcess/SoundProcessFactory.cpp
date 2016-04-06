#include "SoundProcessFactory.hpp"
#include <Process/LayerModel.hpp>
#include <DummyProcess/DummyLayerPanelProxy.hpp>
#include <Audio/CustomEngine/WavBlock.hpp>
#include <Audio/SoundProcess/SoundProcessPresenter.hpp>
#include <Audio/SoundProcess/SoundProcessView.hpp>
namespace Audio
{
namespace SoundProcess
{
QString ProcessFactory::prettyName() const
{ // In factory list
    return Metadata<PrettyName_k, ProcessModel>::get();
}

const UuidKey<Process::ProcessFactory>&ProcessFactory::concreteFactoryKey() const
{
    return Metadata<ConcreteFactoryKey_k, ProcessModel>::get();
}

Process::ProcessModel *ProcessFactory::makeModel(
        const TimeValue& duration,
        const Id<Process::ProcessModel>& id,
        QObject* parent)
{
    return new ProcessModel{duration, id, parent};
}

QByteArray ProcessFactory::makeStaticLayerConstructionData() const
{
    return {};
}

Process::ProcessModel* ProcessFactory::load(
        const VisitorVariant& vis,
        QObject* parent)
{
    return deserialize_dyn(vis, [&] (auto&& deserializer)
    { return new ProcessModel{deserializer, parent};});
}

Process::LayerPresenter *ProcessFactory::makeLayerPresenter(
        const Process::LayerModel& model,
        Process::LayerView* v,
        QObject* parent)
{
    return new LayerPresenter{
        dynamic_cast<const LayerModel&>(model),
                dynamic_cast<LayerView*>(v),
                parent};
}

Process::LayerView* ProcessFactory::makeLayerView(
        const Process::LayerModel&,
        QGraphicsItem* parent)
{
    return new LayerView{parent};
}
}
}
