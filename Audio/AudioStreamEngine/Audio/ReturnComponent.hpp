#pragma once
#include <Audio/AudioStreamEngine/Scenario/ProcessComponent.hpp>

namespace Audio
{
namespace Return
{
class ProcessModel;
}
namespace AudioStreamEngine
{
class ReturnComponent final :
        public ProcessComponent_T<Return::ProcessModel, false, true>
{
        COMPONENT_METADATA("11770cd3-dc49-4d4a-93b2-ab2e5e7faec4")

    public:
       ReturnComponent(
               Return::ProcessModel& sound,
               DocumentPlugin& doc,
                const Id<iscore::Component>& id,
               QObject* parent_obj);

        AudioGraphVertice visit(AudioGraph& graph) override;

       void makeStream(const Context& ctx) override;
};

using ReturnComponentFactory = ProcessComponentFactory_T<ReturnComponent>;
}
}
