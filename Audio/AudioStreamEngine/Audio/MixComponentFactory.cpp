#include "MixComponentFactory.hpp"
#include "MixComponent.hpp"
#include <Audio/MixProcess/MixProcessModel.hpp>

namespace Audio
{
namespace AudioStreamEngine
{
bool MixComponentFactory::matches(
        Process::ProcessModel& p,
        const DocumentPlugin&,
        const iscore::DocumentContext&) const
{
    return dynamic_cast<Mix::ProcessModel*>(&p);
}

ProcessComponent*
MixComponentFactory::make(
        const Id<iscore::Component>& id,
        Process::ProcessModel& proc,
        const DocumentPlugin& doc,
        const iscore::DocumentContext& ctx,
        QObject* paren_objt) const
{
    return new MixComponent(id, static_cast<Mix::ProcessModel&>(proc), doc, ctx, paren_objt);
}

}
}
