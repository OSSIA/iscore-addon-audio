#include "ReturnComponentFactory.hpp"
#include "ReturnComponent.hpp"
#include <Audio/ReturnProcess/ReturnProcessModel.hpp>

namespace Audio
{
namespace AudioStreamEngine
{
bool ReturnComponentFactory::matches(
        Process::ProcessModel& p,
        const DocumentPlugin&,
        const iscore::DocumentContext&) const
{
    return dynamic_cast<Return::ProcessModel*>(&p);
}

ProcessComponent*
ReturnComponentFactory::make(
        const Id<iscore::Component>& id,
        Process::ProcessModel& proc,
        const DocumentPlugin& doc,
        const iscore::DocumentContext& ctx,
        QObject* paren_objt) const
{
    return new ReturnComponent(id, static_cast<Return::ProcessModel&>(proc), doc, ctx, paren_objt);
}

}
}
