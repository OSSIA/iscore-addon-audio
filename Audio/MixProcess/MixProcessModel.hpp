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
#include <Audio/MixProcess/Mix.hpp>

namespace Audio
{
namespace Effect
{
class ProcessModel;
}
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

        const RoutingMap& routings() const
        { return m_routings; }
        const std::list<DirectMix>& directData() const
        { return m_dataProcesses; }
        const std::list<DirectMix>& directFx() const
        { return m_fxProcesses; }
        const std::list<Id<Process::ProcessModel>>& sends() const
        { return m_sendProcesses; }

        double mix(const Routing&) const;
        double mix(
                const Id<Process::ProcessModel>& p1,
                const Id<Process::ProcessModel>& p2) const
        { return mix(Routing{p1, p2, {}}); }

        double mix(const DirectMix&) const;
        double mix(const Id<Process::ProcessModel>& p1) const
        { return mix(DirectMix{p1, {}}); }

        void updateRouting(const Routing&);
        void updateDirectMix(const DirectMix&);

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

        template<typename This_T>
        static auto findDirectMix_impl(This_T& self, const DirectMix& dmx)
        {
            auto it = find(self.m_dataProcesses, dmx.process);
            if(it != self.m_dataProcesses.end())
            {
                return it;
            }
            else
            {
                auto it_2 = find(self.m_fxProcesses, dmx.process);
                if(it_2 != self.m_fxProcesses.end())
                {
                    return it_2;
                }
                else
                {
                    ISCORE_ABORT;
                }
            }

        }

        auto findDirectMix(const DirectMix& dmx)
        {
            return findDirectMix_impl(*this, dmx);
        }
        auto findDirectMix(const DirectMix& dmx) const
        {
            return findDirectMix_impl(*this, dmx);
        }


        RoutingMap m_routings;
        std::list<DirectMix> m_dataProcesses;
        std::list<DirectMix> m_fxProcesses;
        std::list<Id<Process::ProcessModel>> m_sendProcesses;

};
}
}
