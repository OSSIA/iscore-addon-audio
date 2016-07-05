#include "ScenarioComponentFactory.hpp"
#include "ScenarioComponent.hpp"

namespace Audio
{
namespace AudioStreamEngine
{
bool ScenarioComponentFactory::matches(
        Process::ProcessModel& p,
        const DocumentPlugin&,
        const iscore::DocumentContext&) const
{
    return dynamic_cast<Scenario::ProcessModel*>(&p);
}

ProcessComponent*
ScenarioComponentFactory::make(
        const Id<iscore::Component>& id,
        Process::ProcessModel& proc,
        DocumentPlugin& doc,
        const iscore::DocumentContext& ctx,
        QObject* paren_objt) const
{
    return new ScenarioComponent(id, static_cast<Scenario::ProcessModel&>(proc), doc, ctx, paren_objt);
}

}
}
