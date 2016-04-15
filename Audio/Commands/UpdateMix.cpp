#include "UpdateMix.hpp"
#include <Audio/MixProcess/MixProcessModel.hpp>
#include <iscore/tools/SettableIdentifierGeneration.hpp>
#include <iscore/tools/ModelPathSerialization.hpp>
namespace Audio
{
namespace Commands
{

UpdateMix::UpdateMix(
        const Mix::ProcessModel& model,
        Mix::Routing newmix):
    m_model{model},
    m_new{newmix}
{
    auto it = model.routings().find(newmix);
    ISCORE_ASSERT(it != model.routings().end());
    m_old = *it;
}

void UpdateMix::undo() const
{
    Mix::ProcessModel& process = m_model.find();
    process.updateRouting(m_old);
}

void UpdateMix::redo() const
{
    Mix::ProcessModel& process = m_model.find();
    process.updateRouting(m_new);
}

void UpdateMix::serializeImpl(DataStreamInput& s) const
{
    ISCORE_TODO;
}

void UpdateMix::deserializeImpl(DataStreamOutput& s)
{
    ISCORE_TODO;
}

}
}
