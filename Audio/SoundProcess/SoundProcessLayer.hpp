#pragma once
#include <Process/LayerModel.hpp>

#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
namespace Audio
{
namespace Sound
{
class ProcessModel;
class Layer final : public Process::LayerModel
{
        ISCORE_SERIALIZE_FRIENDS(Layer, DataStream)
        ISCORE_SERIALIZE_FRIENDS(Layer, JSONObject)

    public:
        explicit Layer(
                ProcessModel& model,
                const Id<Process::LayerModel>& id,
                QObject* parent);

        // Copy
        explicit Layer(
                const Layer& source,
                ProcessModel& model,
                const Id<Process::LayerModel>& id,
                QObject* parent);

        // Load
        template<typename Impl>
        explicit Layer(
                Deserializer<Impl>& vis,
                ProcessModel& model,
                QObject* parent) :
            Process::LayerModel {vis, model, parent}
        {
            vis.writeTo(*this);
        }

        void serialize_impl(const VisitorVariant&) const override;
};

}
}
