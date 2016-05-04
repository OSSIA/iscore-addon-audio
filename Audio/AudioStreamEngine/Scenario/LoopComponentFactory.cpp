#include "LoopComponentFactory.hpp"
#include "LoopComponent.hpp"

namespace Audio
{
namespace AudioStreamEngine
{
bool LoopComponentFactory::matches(
        Process::ProcessModel& p,
        const DocumentPlugin&,
        const iscore::DocumentContext&) const
{
    return dynamic_cast<Scenario::ScenarioModel*>(&p);
}

ProcessComponent*
LoopComponentFactory::make(
        const Id<iscore::Component>& id,
        Process::ProcessModel& proc,
        const DocumentPlugin& doc,
        const iscore::DocumentContext& ctx,
        QObject* paren_objt) const
{
    return new LoopComponent(id, static_cast<Scenario::ScenarioModel&>(proc), doc, ctx, paren_objt);
}

}
}
