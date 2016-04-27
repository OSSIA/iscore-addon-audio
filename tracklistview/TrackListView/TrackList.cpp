#include "TrackList.hpp"
#include "DeviceInfo.hpp"

Track::Track() : m_vol(1), m_pan(0), m_output(0) {}

double Track::volume() {
    return m_vol;
}
double Track::pan() {
    return m_pan;
}
long Track::output() {
    return m_output;
}

void Track::setVolume(double v) {
    if (v >= 0 && v <= 1)
        m_vol = v;
}

void Track::setPan(double p) {
    if (p >= -1 && p <= 1)
        m_pan = p;
}

void Track::setOutput(long o) {
    if (o >= 0 && o < MAX_OUTPUT)
        m_output = o;
}

TrackModel::TrackModel(QObject *parent) {

}

QHash<int, QByteArray> TrackModel::roleNames() {
    QHash<int, QByteArray> roles;
    roles[VolRole] = "vol";
    roles[PanRole] = "pan";
    roles[OutRole] = "out";
    return roles;
}

void TrackModel::insertRows() {
}

int TrackModel::rowCount(const QModelIndex& m) const {
    return m_data.size();
}

Qt::ItemFlags TrackModel::flags(int index) {
    return Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

void TrackModel::addTrack(Track t, int index) {

}
