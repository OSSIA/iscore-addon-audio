#pragma once
#include <Audio/AudioStreamEngine/Scenario/ProcessComponent.hpp>

namespace Audio
{
namespace AudioStreamEngine
{

class ReturnComponentFactory final :
        public ProcessComponentFactory
{
        ISCORE_CONCRETE_FACTORY_DECL("f8d4cbe3-b187-43ed-8831-2510b4cffb01")
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
