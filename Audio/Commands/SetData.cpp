#include "SetData.hpp"

namespace Audio {
namespace Commands {

SetData::SetData(Path<Panel::TrackModel> device_tree,
                 int index,
                 QVariant value,
                 Panel::TrackModel::TrackRoles role) :
    m_devicesModel{device_tree},
    m_index(index),
    m_newval(value),
    m_oldval{m_devicesModel.find().data(m_devicesModel.find().index(index), role)},
    m_role(role)
{

}

void SetData::undo() const {
    auto& tm = m_devicesModel.find();
    tm.setData(tm.index(m_index), m_oldval, m_role);
}

void SetData::redo() const {
    auto& tm = m_devicesModel.find();
    tm.setData(tm.index(m_index), m_newval, m_role);
}

void SetData::serializeImpl(DataStreamInput &s) const {
    s << m_index
      << m_oldval
      << m_newval
      << m_role;
}

void SetData::deserializeImpl(DataStreamOutput &s) {
    s >> m_index
      >> m_oldval
      >> m_newval
      >> m_role;
}

}
}
