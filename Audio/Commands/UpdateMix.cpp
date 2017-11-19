#include "UpdateMix.hpp"
#include <Audio/MixProcess/MixProcessModel.hpp>
#include <score/tools/IdentifierGeneration.hpp>
#include <score/model/path/PathSerialization.hpp>
namespace Audio
{
namespace Commands
{

UpdateRouting::UpdateRouting(
        const Mix::ProcessModel& model,
        Mix::Routing newmix):
    m_model{model},
    m_new{newmix}
{
    auto it = model.routings().find(newmix);
    SCORE_ASSERT(it != model.routings().end());
    m_old = *it;
}

void UpdateRouting::undo(const score::DocumentContext& ctx) const
{
    Mix::ProcessModel& process = m_model.find(ctx);
    process.updateRouting(m_old);
}

void UpdateRouting::redo(const score::DocumentContext& ctx) const
{
    Mix::ProcessModel& process = m_model.find(ctx);
    process.updateRouting(m_new);
}

void UpdateRouting::serializeImpl(DataStreamInput& s) const
{
    s << m_model << m_old << m_new;
}

void UpdateRouting::deserializeImpl(DataStreamOutput& s)
{
    s >> m_model >> m_old >> m_new;
}


UpdateDirect::UpdateDirect(
        const Mix::ProcessModel& model,
        Mix::DirectMix newmix):
    m_model{model},
    m_new{newmix}
{
    auto it = ossia::find(model.directData(), newmix);
    if(it != model.directData().end())
    {
        m_old = *it;
    }
    else
    {
        auto it2 = ossia::find(model.directFx(), newmix);
        if(it2 != model.directFx().end())
        {
            m_old = *it;
        }
        else
        {
            SCORE_ABORT;
        }
    }
}

void UpdateDirect::undo(const score::DocumentContext& ctx) const
{
    Mix::ProcessModel& process = m_model.find(ctx);
    process.updateDirectMix(m_old);
}

void UpdateDirect::redo(const score::DocumentContext& ctx) const
{
    Mix::ProcessModel& process = m_model.find(ctx);
    process.updateDirectMix(m_new);
}

void UpdateDirect::serializeImpl(DataStreamInput& s) const
{
    s << m_model << m_old << m_new;
}

void UpdateDirect::deserializeImpl(DataStreamOutput& s)
{
    s >> m_model >> m_old >> m_new;
}

}
}
