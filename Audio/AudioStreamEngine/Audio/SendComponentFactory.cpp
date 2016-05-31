#include "SendComponentFactory.hpp"
#include "SendComponent.hpp"
#include <Audio/SendProcess/SendProcessModel.hpp>

namespace Audio
{
namespace AudioStreamEngine
{
bool SendComponentFactory::matches(
        Process::ProcessModel& p,
        const DocumentPlugin&,
        const iscore::DocumentContext&) const
{
    return dynamic_cast<Send::ProcessModel*>(&p);
}

ProcessComponent*
SendComponentFactory::make(
        const Id<iscore::Component>& id,
        Process::ProcessModel& proc,
        DocumentPlugin& doc,
        const iscore::DocumentContext& ctx,
        QObject* paren_objt) const
{
    return new SendComponent(id, static_cast<Send::ProcessModel&>(proc), doc, ctx, paren_objt);
}

}
}
