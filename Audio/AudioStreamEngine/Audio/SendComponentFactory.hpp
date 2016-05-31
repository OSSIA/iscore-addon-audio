#pragma once
#include <Audio/AudioStreamEngine/Scenario/ProcessComponent.hpp>

namespace Audio
{
namespace AudioStreamEngine
{

class SendComponentFactory final :
        public ProcessComponentFactory
{
        ISCORE_CONCRETE_FACTORY_DECL("2340bde2-3226-48ce-960b-5243563e026a")
        bool matches(
                Process::ProcessModel& p,
                const Audio::AudioStreamEngine::DocumentPlugin&,
                const iscore::DocumentContext&) const override;

        ProcessComponent* make(
                const Id<iscore::Component>& id,
                Process::ProcessModel& proc,
                DocumentPlugin& doc,
                const iscore::DocumentContext& ctx,
                QObject* paren_objt) const override;
};
}
}
