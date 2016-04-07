#include "SoundProcessLayer.hpp"
#include "SoundProcessModel.hpp"
namespace Audio
{
namespace Sound
{

LayerModel::LayerModel(
        ProcessModel& model,
        const Id<Process::LayerModel>& id,
        QObject* parent):
    Process::LayerModel{id, "LayerModel", model, parent}
{
}

LayerModel::LayerModel(
        const LayerModel&,
        ProcessModel& model,
        const Id<Process::LayerModel>& id,
        QObject* parent):
    Process::LayerModel{id, "LayerModel", model, parent}
{

}

void LayerModel::serialize(const VisitorVariant& vis) const
{
    serialize_dyn(vis, *this);
}

Process::LayerModelPanelProxy* LayerModel::make_panelProxy(QObject* parent) const
{
    return new Dummy::DummyLayerPanelProxy{*this, parent};
}


}
}
