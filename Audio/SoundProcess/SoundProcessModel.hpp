#pragma once
#include <Process/Process.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <iscore/serialization/VisitorCommon.hpp>
#include <Audio/SoundProcess/SoundProcessMetadata.hpp>
#include <DummyProcess/DummyLayerPanelProxy.hpp>
#include <Process/LayerModel.hpp>
#include <Audio/MediaFileHandle.hpp>


namespace Audio
{
namespace SoundProcess
{
class ProcessModel;

class ProcessModel final : public Process::ProcessModel
{
        ISCORE_SERIALIZE_FRIENDS(Audio::SoundProcess::ProcessModel, DataStream)
        ISCORE_SERIALIZE_FRIENDS(Audio::SoundProcess::ProcessModel, JSONObject)

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

        void setFile(const QString& file);
        void setFile(const MediaFileHandle& file);

        const MediaFileHandle& file() const
        { return m_file; }

        // Process interface
        ProcessModel* clone(
                const Id<Process::ProcessModel>& newId,
                QObject* newParent) const override;

        UuidKey<Process::ProcessFactory> concreteFactoryKey() const override
        {
            return Metadata<ConcreteFactoryKey_k, Audio::SoundProcess::ProcessModel>::get();
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
        void fileChanged();

    protected:
        Process::LayerModel* makeLayer_impl(
                const Id<Process::LayerModel>& viewModelId,
                const QByteArray& constructionData,
                QObject* parent) override;
        Process::LayerModel* loadLayer_impl(const VisitorVariant&, QObject* parent) override;
        Process::LayerModel* cloneLayer_impl(const Id<Process::LayerModel>& newId, const Process::LayerModel& source, QObject* parent) override;

    private:
        MediaFileHandle m_file;
};
}
}
