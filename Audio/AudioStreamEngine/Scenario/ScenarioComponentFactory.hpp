#pragma once
#include <Audio/AudioStreamEngine/Scenario/ProcessComponent.hpp>

namespace Audio
{
namespace AudioStreamEngine
{

class ScenarioComponentFactory final :
        public ProcessComponentFactory
{
        ISCORE_CONCRETE_FACTORY_DECL("34f7cb5f-fe0e-4dd8-8250-956702e41e13")
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
