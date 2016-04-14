#pragma once
#include <Process/Process.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <iscore/serialization/VisitorCommon.hpp>
#include <Audio/MixProcess/MixProcessMetadata.hpp>
#include <DummyProcess/DummyLayerPanelProxy.hpp>
#include <Process/LayerModel.hpp>
#include <Audio/MediaFileHandle.hpp>
#include <unordered_set>

#include <boost/multi_index/identity.hpp>

namespace Audio
{
namespace Effect
{
class ProcessModel;
}

namespace Mix
{
struct Routing
{
        friend bool operator==(const Routing& lhs, const Routing& rhs)
        {
            return lhs.in == rhs.in && lhs.out == rhs.out;
        }

        Id<Process::ProcessModel> in;
        Id<Process::ProcessModel> out;

        double mix{};
};

using RoutingMap = bmi::multi_index_container<
Routing,
bmi::indexed_by<
    bmi::hashed_unique<
        bmi::identity<Routing>
    >
>
>;
}
}


namespace std {
template<>
struct hash<Audio::Mix::Routing> {
public:
    auto operator()(const Audio::Mix::Routing &s) const
    {
        auto h1 = id_hash<Process::ProcessModel>()(s.in);
        auto h2 = id_hash<Process::ProcessModel>()(s.out);
        return h1 ^ ( h2 << 1 );
    }
};
}


namespace boost {
template<>
struct hash<Audio::Mix::Routing> {
public:
    auto operator()(const Audio::Mix::Routing &s) const
    {
        auto h1 = id_hash<Process::ProcessModel>()(s.in);
        auto h2 = id_hash<Process::ProcessModel>()(s.out);
        return h1 ^ ( h2 << 1 );
    }
};
}




namespace Audio
{
namespace Mix
{
/**
 * @brief The Mix::ProcessModel class
 *
 * Cases to handle carefully :
 * - Serialization
 * - Copy, cloning
 * - Recreation (undo - redo)
 */
class ProcessModel final :
        public Process::ProcessModel,
        public Nano::Observer
{
        ISCORE_SERIALIZE_FRIENDS(Audio::Mix::ProcessModel, DataStream)
        ISCORE_SERIALIZE_FRIENDS(Audio::Mix::ProcessModel, JSONObject)

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

        void updateRouting(const Routing&);

        // Process interface
        ProcessModel* clone(
                const Id<Process::ProcessModel>& newId,
                QObject* newParent) const override;

        UuidKey<Process::ProcessFactory> concreteFactoryKey() const override
        {
            return Metadata<ConcreteFactoryKey_k, Audio::Mix::ProcessModel>::get();
        }

        QString prettyName() const override;
        QByteArray makeLayerConstructionData() const override;

        void setDurationAndScale(const TimeValue& newDuration) override;
        void setDurationAndGrow(const TimeValue& newDuration) override;
        void setDurationAndShrink(const TimeValue& newDuration) override;

        void startExecution() override;
        void stopExecution() override;
        void reset() override;

        ProcessStateDataInterface* startStateData() const override;
        ProcessStateDataInterface* endStateData() const override;

        Selection selectableChildren() const override;
        Selection selectedChildren() const override;
        void setSelection(const Selection& s) const override;

        void serialize_impl(const VisitorVariant& vis) const override;

    signals:
        void routingChanged();

    protected:
        Process::LayerModel* makeLayer_impl(
                const Id<Process::LayerModel>& viewModelId,
                const QByteArray& constructionData,
                QObject* parent) override;
        Process::LayerModel* loadLayer_impl(const VisitorVariant&, QObject* parent) override;
        Process::LayerModel* cloneLayer_impl(const Id<Process::LayerModel>& newId, const Process::LayerModel& source, QObject* parent) override;

    private:
        void init();
        void on_processAdded(const Process::ProcessModel&);
        void on_processRemoved(const Process::ProcessModel&);
        RoutingMap m_routings;
        std::list<Id<Process::ProcessModel>> m_dataProcesses;
        std::list<Id<Process::ProcessModel>> m_fxProcesses;

};
}
}
