#include "ChangeSend.hpp"

#include <iscore/model/path/PathSerialization.hpp>
#include <Audio/SendProcess/SendProcessModel.hpp>
#include <Audio/ReturnProcess/ReturnProcessModel.hpp>
namespace Audio
{
namespace Commands
{

ChangeSend::ChangeSend(
    const Return::ProcessModel& model,
    const Path<Send::ProcessModel>& text):
    m_model{model},
    m_new{text}
{
    m_old = model.send();
}

void ChangeSend::undo(const iscore::DocumentContext& ctx) const
{
    m_model.find(ctx).setSend(m_old);
}

void ChangeSend::redo(const iscore::DocumentContext& ctx) const
{
    m_model.find(ctx).setSend(m_new);
}

void ChangeSend::serializeImpl(DataStreamInput& s) const
{
    s << m_model << m_old << m_new;
}

void ChangeSend::deserializeImpl(DataStreamOutput& s)
{
    s >> m_model >> m_old >> m_new;
}

}
}
