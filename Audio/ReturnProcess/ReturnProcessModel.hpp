#pragma once
#include <Process/Process.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <iscore/serialization/VisitorCommon.hpp>
#include <Audio/ReturnProcess/ReturnProcessMetadata.hpp>
#include <unordered_set>

#include <boost/multi_index/identity.hpp>

namespace Audio
{
namespace Send
{
class ProcessModel;
}
namespace Return
{
class ProcessModel final :
        public Process::ProcessModel,
        public Nano::Observer
{
        ISCORE_SERIALIZE_FRIENDS
        PROCESS_METADATA_IMPL(Audio::Return::ProcessModel)

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
        }

        const Path<Send::ProcessModel>& send() const
        { return m_sendPath; }

        const Send::ProcessModel* send_ptr(const iscore::DocumentContext& ctx) const;

        void setSend(const Path<Send::ProcessModel>& send)
        {
            if(send != m_sendPath)
            {
                m_sendPath = send;
                emit sendChanged();
            }
        }

    signals:
        void routingChanged();
        void sendChanged();

    private:
        Path<Send::ProcessModel> m_sendPath; // TODO ought to be able to work with downcast Id / Path

};
}
}
