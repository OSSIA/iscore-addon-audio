#include "AudioFactory.hpp"

QString Audio::ProcessFactory::prettyName() const
{ // In factory list
    return Metadata<PrettyName_k, Audio::ProcessModel>::get();
}

const UuidKey<Process::ProcessFactory>&Audio::ProcessFactory::concreteFactoryKey() const
{
    return Metadata<ConcreteFactoryKey_k, Audio::ProcessModel>::get();
}

Process::ProcessModel *Audio::ProcessFactory::makeModel(
        const TimeValue& duration,
        const Id<Process::ProcessModel>& id,
        QObject* parent)
{
    return new Audio::ProcessModel{duration, id, parent};
}

QByteArray Audio::ProcessFactory::makeStaticLayerConstructionData() const
{
    return {};
}

Process::ProcessModel* Audio::ProcessFactory::load(
        const VisitorVariant& vis,
        QObject* parent)
{
    return deserialize_dyn(vis, [&] (auto&& deserializer)
    { return new Audio::ProcessModel{deserializer, parent};});
}

Process::LayerPresenter *Audio::ProcessFactory::makeLayerPresenter(
        const Process::LayerModel& model,
        Process::LayerView* v,
        QObject* parent)
{
    return new Dummy::DummyLayerPresenter{
        model, dynamic_cast<Dummy::DummyLayerView*>(v), parent};
}

Process::LayerView* Audio::ProcessFactory::makeLayerView(
        const Process::LayerModel&,
        QGraphicsItem* parent)
{
    return new Dummy::DummyLayerView{parent};
}
