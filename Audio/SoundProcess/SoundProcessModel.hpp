#pragma once
#include <Process/Process.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <iscore/serialization/VisitorCommon.hpp>
#include <Audio/SoundProcess/SoundProcessMetadata.hpp>
#include <Audio/MediaFileHandle.hpp>
#include <iscore_plugin_audio_export.h>


namespace Audio
{
namespace Sound
{
class ProcessModel;

class ISCORE_PLUGIN_AUDIO_EXPORT ProcessModel final : public Process::ProcessModel
{
        ISCORE_SERIALIZE_FRIENDS
        PROCESS_METADATA_IMPL(Audio::Sound::ProcessModel)

        Q_OBJECT
    public:
        explicit ProcessModel(
                const TimeVal& duration,
                const Id<Process::ProcessModel>& id,
                QObject* parent);

        explicit ProcessModel(
                const ProcessModel& source,
                const Id<Process::ProcessModel>& id,
                QObject* parent);

        virtual ~ProcessModel();

        template<typename Impl>
        explicit ProcessModel(
                Impl& vis,
                QObject* parent) :
            Process::ProcessModel{vis, parent}
        {
            vis.writeTo(*this);
            init();
        }

        void setFile(const QString& file);
        void setFile(const MediaFileHandle& file);

        MediaFileHandle& file()
        { return m_file; }
        const MediaFileHandle& file() const
        { return m_file; }

    signals:
        void fileChanged();

    private:
        void init();

        MediaFileHandle m_file;
};

}
}
