#pragma once
#include <Process/Process.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <iscore/serialization/VisitorCommon.hpp>
#include <Audio/InputProcess/InputProcessMetadata.hpp>
#include <Process/Dummy/DummyLayerPanelProxy.hpp>
#include <Process/LayerModel.hpp>
#include <Audio/MediaFileHandle.hpp>
#include <unordered_set>

#include <boost/multi_index/identity.hpp>

namespace Audio
{
namespace Input
{
class ProcessModel final :
        public Process::ProcessModel,
        public Nano::Observer
{
        ISCORE_SERIALIZE_FRIENDS(Audio::Input::ProcessModel, DataStream)
        ISCORE_SERIALIZE_FRIENDS(Audio::Input::ProcessModel, JSONObject)

        Q_OBJECT
    public:
        explicit ProcessModel(
                const TimeValue& duration,
                const Id<Process::ProcessModel>& id,
                QObject* parent);

        explicit ProcessModel(
                const ProcessModel& source,
                const Id<Process::ProcessModel>& id,
                QObject* parent);

        virtual ~ProcessModel();

        template<typename Impl>
        explicit ProcessModel(
                Deserializer<Impl>& vis,
                QObject* parent) :
            Process::ProcessModel{vis, parent}
        {
            vis.writeTo(*this);
        }

        // Process interface
        ProcessModel* clone(
                const Id<Process::ProcessModel>& newId,
                QObject* newParent) const override;

        UuidKey<Process::ProcessFactory> concreteFactoryKey() const override
        {
            return Metadata<ConcreteFactoryKey_k, Audio::Input::ProcessModel>::get();
        }

        QString prettyName() const override;

        void serialize_impl(const VisitorVariant& vis) const override;

    signals:
        void routingChanged();

};
}
}
