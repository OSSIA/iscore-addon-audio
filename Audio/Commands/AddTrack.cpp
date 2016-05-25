#include "AddTrack.hpp"
#include <Audio/Panel/TrackModel.hpp>

namespace Audio {
namespace Commands {

AddTrack::AddTrack(Path<Panel::TrackModel> device_tree, const Panel::Track& t) :
    m_devicesModel{device_tree},
    m_index{m_devicesModel.find().rowCount()},
    m_track{t}
{
    qDebug() << m_devicesModel.find().parent();
    qDebug() << "created new AddTrack with index" << m_index;
}

void AddTrack::undo() const {
    qDebug() << "undoing AddTrack...";
    auto& tm = m_devicesModel.find();
    qDebug() << "found model:" << &tm;
    tm.removeRows(createdTrackIndex(), 1);
    qDebug() << "done.";
}

void AddTrack::redo() const {
    qDebug() << "redoing AddTrack...";
    auto& tm = m_devicesModel.find();

    if (tm.insertRows(tm.rowCount(), 1)) {
        QModelIndex index = tm.index(createdTrackIndex());
        tm.setData(index, m_track.pan(), Panel::TrackModel::PanRole);
        tm.setData(index, (int)(m_track.out()), Panel::TrackModel::OutRole);
        tm.setData(index, QVariant(m_track.vol()), Panel::TrackModel::VolRole);
    }
    qDebug() << "done.";
    qDebug() << &tm;
    tm.print();
}

int AddTrack::createdTrackIndex() const {
    return m_index;
}

void AddTrack::serializeImpl(DataStreamInput &s) const {
    s << m_index
      << m_track;
}

void AddTrack::deserializeImpl(DataStreamOutput &s) {
    s >> m_index
      >> m_track;
}

}
}
