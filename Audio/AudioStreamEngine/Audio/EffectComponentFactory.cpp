#include "EffectComponentFactory.hpp"
#include "EffectComponent.hpp"
#include <Audio/EffectProcess/EffectProcessModel.hpp>

namespace Audio
{
namespace AudioStreamEngine
{
bool EffectComponentFactory::matches(
        Process::ProcessModel& p,
        const DocumentPlugin&,
        const iscore::DocumentContext&) const
{
    return dynamic_cast<Effect::ProcessModel*>(&p);
}

ProcessComponent*
EffectComponentFactory::make(
        const Id<iscore::Component>& id,
        Process::ProcessModel& proc,
        const DocumentPlugin& doc,
        const iscore::DocumentContext& ctx,
        QObject* paren_objt) const
{
    return new EffectProcessComponent(id, static_cast<Effect::ProcessModel&>(proc), doc, ctx, paren_objt);
}

}
}
