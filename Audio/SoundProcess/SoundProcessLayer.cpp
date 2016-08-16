#include "SoundProcessLayer.hpp"
#include "SoundProcessModel.hpp"
namespace Audio
{
namespace Sound
{

Layer::Layer(
        ProcessModel& model,
        const Id<Process::LayerModel>& id,
        QObject* parent):
    Process::LayerModel{id, "LayerModel", model, parent}
{
}

Layer::Layer(
        const Layer&,
        ProcessModel& model,
        const Id<Process::LayerModel>& id,
        QObject* parent):
    Process::LayerModel{id, "LayerModel", model, parent}
{

}

void Layer::serialize_impl(const VisitorVariant& vis) const
{
    serialize_dyn(vis, *this);
}

}
}
