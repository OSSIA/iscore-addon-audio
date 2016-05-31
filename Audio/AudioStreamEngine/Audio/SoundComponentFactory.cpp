#include "SoundComponentFactory.hpp"
#include "SoundComponent.hpp"
#include <Audio/SoundProcess/SoundProcessModel.hpp>

namespace Audio
{
namespace AudioStreamEngine
{
bool SoundComponentFactory::matches(
        Process::ProcessModel& p,
        const DocumentPlugin&,
        const iscore::DocumentContext&) const
{
    return dynamic_cast<Sound::ProcessModel*>(&p);
}

ProcessComponent*
SoundComponentFactory::make(
        const Id<iscore::Component>& id,
        Process::ProcessModel& proc,
        DocumentPlugin& doc,
        const iscore::DocumentContext& ctx,
        QObject* paren_objt) const
{
    return new SoundComponent(id, static_cast<Sound::ProcessModel&>(proc), doc, ctx, paren_objt);
}

}
}
