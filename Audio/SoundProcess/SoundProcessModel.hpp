#pragma once
#include <Process/Process.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <iscore/serialization/VisitorCommon.hpp>
#include <Audio/SoundProcess/SoundProcessMetadata.hpp>
#include <Process/Dummy/DummyLayerPanelProxy.hpp>
#include <Process/LayerModel.hpp>
#include <Audio/MediaFileHandle.hpp>


namespace Audio
{
namespace Sound
{
class ProcessModel;

class ProcessModel final : public Process::ProcessModel
{
        ISCORE_SERIALIZE_FRIENDS(Audio::Sound::ProcessModel, DataStream)
        ISCORE_SERIALIZE_FRIENDS(Audio::Sound::ProcessModel, JSONObject)

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
            return Metadata<ConcreteFactoryKey_k, Audio::Sound::ProcessModel>::get();
        }

        QString prettyName() const override;

        void serialize_impl(const VisitorVariant& vis) const override;

    signals:
        void fileChanged();

    private:
        MediaFileHandle m_file;
};
}
}
