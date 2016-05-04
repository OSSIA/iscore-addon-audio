#include "ChangeSend.hpp"

#include <iscore/tools/ModelPathSerialization.hpp>
#include <Audio/SendProcess/SendProcessModel.hpp>
#include <Audio/ReturnProcess/ReturnProcessModel.hpp>
namespace Audio
{
namespace Commands
{

ChangeSend::ChangeSend(
        Path<Return::ProcessModel>&& model,
        const Path<Send::ProcessModel>& text):
    m_model{std::move(model)},
    m_new{text}
{
    m_old = m_model.find().send();
}

void ChangeSend::undo() const
{
    m_model.find().setSend(m_old);
}

void ChangeSend::redo() const
{
    m_model.find().setSend(m_new);
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
