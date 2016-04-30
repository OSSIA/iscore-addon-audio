#include "TrackList.hpp"
#include "DeviceInfo.hpp"
#include <QDebug>

Track::Track(double volval, double panval, long outval) : m_vol(volval), m_pan(panval), m_output(outval) {}
Track::Track() : Track(100, 0, 0) {}

double Track::vol() const {
    return m_vol;
}
double Track::pan() const {
    return m_pan;
}
long Track::out() const {
    return m_output;
}

void Track::setVol(double v) {
    if (v >= 0 && v <= 100)
        m_vol = v;
}

void Track::setPan(double p) {
    if (p >= -1 && p <= 1)
        m_pan = p;
}

void Track::setOut(long o) {
    if (o >= 0 && o < MAX_OUTPUT)
        m_output = o;
}

TrackModel::TrackModel(QObject *parent) : QAbstractListModel(parent) {}

void TrackModel::addTrackSignal() {
    addTrack(Track());
}

QHash<int, QByteArray> TrackModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[VolRole] = "vol";
    roles[PanRole] = "pan";
    roles[OutRole] = "out";
    return roles;
}

bool TrackModel::insertRows(int row, int count, const QModelIndex& parent) {
    beginInsertRows(parent, row, row + count - 1);
    m_data.insert(m_data.begin() + row, count, Track());
    endInsertRows();
    return true;
}

bool TrackModel::removeRows(int row, int count, const QModelIndex &parent) {
    beginRemoveRows(parent, row, row + count - 1);
    auto it = m_data.begin();
    m_data.erase(it + row, it + row + count);
    endRemoveRows();
    return true;
}

QVariant TrackModel::data(const QModelIndex &index, int role) const {
    int i = index.row();
    if (i >= m_data.size() || i < 0)
        return QVariant(-10);
    else {
        Track t = m_data[i];
        switch (role) {
        case VolRole:
            return t.vol();
        case PanRole:
            return t.pan();
        case OutRole:
            return (int)t.out();
        }
    }
}

int TrackModel::rowCount(const QModelIndex& m) const {
    return m_data.size();
}

Qt::ItemFlags TrackModel::flags(int index) {
    return Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool TrackModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    int i = index.row();
    switch (role) {
    case VolRole:
        m_data[i].setVol(value.toDouble());
        break;
    case PanRole:
        m_data[i].setPan(value.toDouble());
        break;
    case OutRole:
        m_data[i].setOut((long)value.toLongLong());
        break;
     default:
        return false;
    }

    QVector<int> roles = QVector<int> (1, role);
    emit(QAbstractListModel::dataChanged(index, index, roles));
    return true;
}

void TrackModel::addTrack(const Track& t) {
    if (insertRows(rowCount(), 1)) {
        QModelIndex index = TrackModel::index(rowCount() - 1);
        setData(index, t.pan(), PanRole);
        setData(index, (int)(t.out()), OutRole);
        setData(index, QVariant(t.vol()), VolRole);
    }
}

void TrackModel::removeTrack(int index) {
    removeRows(index, 1);
}

double TrackModel::getVol(int i) const {
    int val = data(index(i), VolRole).toInt();
    return (double)val;
}

double TrackModel::getPan(int i) const {
    return data(index(i), PanRole).toDouble();
}

long TrackModel::getOut(int i) const {
    int o = data(index(i), OutRole).toDouble();
    return (long)o;
}

void TrackModel::setVol(int i, double volval) {
    setData(index(i), volval, VolRole);
}

void TrackModel::setPan(int i, double panval) {
    setData(index(i), panval, PanRole);
}

void TrackModel::setOut(int i, long outval) {
    setData(index(i), (int)outval, OutRole);
}

void TrackModel::print() {
    int s = m_data.size();
    for (int i = 0; i < s; ++i) {
        Track t = m_data[i];
    }
}
