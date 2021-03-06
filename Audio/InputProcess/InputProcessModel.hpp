#pragma once
#include <Process/Process.hpp>
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>
#include <score/serialization/VisitorCommon.hpp>
#include <Audio/InputProcess/InputProcessMetadata.hpp>
#include <unordered_set>

namespace Audio
{
namespace Input
{
struct InputChannels
{
        std::vector<long> inputs;
};

class ProcessModel final :
        public Process::ProcessModel,
        public Nano::Observer
{
        SCORE_SERIALIZE_FRIENDS
        PROCESS_METADATA_IMPL(Audio::Input::ProcessModel)
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

        InputChannels inputs() const
        { return {{0, 1}}; }

};
}
}
