#include "RemoveTrack.hpp"
#include <Audio/Panel/TrackModel.hpp>

namespace Audio {
namespace Commands {

RemoveTrack::RemoveTrack(Path<Panel::TrackModel> device_tree, int index) :
    m_devicesModel{device_tree},
    m_index{index},
    m_track{Panel::Track(m_devicesModel.find().getVol(index),
                         m_devicesModel.find().getPan(index),
                         m_devicesModel.find().getOut(index))}
{
    qDebug() << m_devicesModel.find().parent();
    qDebug() << "created new RemoveTrack with index" << m_index;
}

void RemoveTrack::undo() const {
    qDebug() << "undoing RemoveTrack";
    auto& tm = m_devicesModel.find();
    if (tm.insertRows(m_index, 1)) {
        QModelIndex index = tm.index(m_index);
        tm.setData(index, m_track.pan(), Panel::TrackModel::PanRole);
        tm.setData(index, (int)(m_track.out()), Panel::TrackModel::OutRole);
        tm.setData(index, QVariant(m_track.vol()), Panel::TrackModel::VolRole);
    }
}

void RemoveTrack::redo() const {
    qDebug() << "redoing RemoveTrack...";
    auto& tm = m_devicesModel.find();
    tm.removeRows(m_index, 1);
}

void RemoveTrack::serializeImpl(DataStreamInput &s) const {
    s << m_index
      << m_track;
}

void RemoveTrack::deserializeImpl(DataStreamOutput &s) {
    s >> m_index
      >> m_track;
}

}
}
