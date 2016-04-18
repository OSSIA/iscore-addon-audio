#include "InputProcessFactory.hpp"
#include <Process/LayerModel.hpp>
#include <DummyProcess/DummyLayerPanelProxy.hpp>
namespace Audio
{
namespace Input
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
    return new Dummy::DummyLayerPresenter{model, dynamic_cast<Dummy::DummyLayerView*>(v), parent};
}

Process::LayerView* ProcessFactory::makeLayerView(
        const Process::LayerModel&,
        QGraphicsItem* parent)
{
    return new Dummy::DummyLayerView{parent};
}
}
}
