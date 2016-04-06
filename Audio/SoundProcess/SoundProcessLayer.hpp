#pragma once
#include <Process/LayerModel.hpp>

namespace Audio
{
namespace SoundProcess
{
class ProcessModel;
class LayerModel final : public Process::LayerModel
{
        ISCORE_SERIALIZE_FRIENDS(LayerModel, DataStream)
        ISCORE_SERIALIZE_FRIENDS(LayerModel, JSONObject)

    public:
        explicit LayerModel(
                ProcessModel& model,
                const Id<Process::LayerModel>& id,
                QObject* parent);

        // Copy
        explicit LayerModel(
                const LayerModel& source,
                ProcessModel& model,
                const Id<Process::LayerModel>& id,
                QObject* parent);

        // Load
        template<typename Impl>
        explicit LayerModel(
                Deserializer<Impl>& vis,
                ProcessModel& model,
                QObject* parent) :
            Process::LayerModel {vis, model, parent}
        {
            vis.writeTo(*this);
        }

        void serialize(const VisitorVariant&) const override;
        Process::LayerModelPanelProxy* make_panelProxy(QObject* parent) const override;
};

}
}
