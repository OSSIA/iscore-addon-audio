#pragma once
#include <Process/Process.hpp>
#include <Audio/GenProcess/GenProcessMetadata.hpp>

#include <LibAudioStreamMC++.h>
namespace Audio
{
namespace Gen
{
class ProcessModel final :
        public Process::ProcessModel,
        public Nano::Observer
{
        ISCORE_SERIALIZE_FRIENDS
        MODEL_METADATA_IMPL(Audio::Gen::ProcessModel)
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


        const QString& text() const
        {
            return m_text;
        }

        void setText(const QString& txt);

    signals:
        void textChanged();

    private:
        void init();
        void reload();
        QString m_text;
        AudioEffect m_effect;

};
}
}
