#pragma once
#include <Process/Process.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <iscore/serialization/VisitorCommon.hpp>
#include <Audio/InputProcess/InputProcessMetadata.hpp>
#include <Process/LayerModel.hpp>
#include <Audio/MediaFileHandle.hpp>
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
        ISCORE_SERIALIZE_FRIENDS
        MODEL_METADATA_IMPL(Audio::Input::ProcessModel)
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
