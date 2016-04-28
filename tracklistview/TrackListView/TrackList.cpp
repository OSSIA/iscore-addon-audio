#include "TrackList.hpp"
#include "DeviceInfo.hpp"
#include <QDebug>

Track::Track() : Track(100, 0, 0) {}
Track::Track(double volval, double panval, long outval) : m_vol(volval), m_pan(panval), m_output(outval) {
    qDebug() << "Created track: vol =" << vol() << "pan =" << pan() << "out =" << out();
}

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
    if (v >= 0 && v <= 1)
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
    beginInsertRows(parent, row - count, row - 1);
    m_data.insert(m_data.begin() + row, count, Track());
    endInsertRows();
    return true;
}

bool TrackModel::removeRows(int row, int count, const QModelIndex &parent) {
    beginRemoveRows(parent, row, row + count - 1);
    auto it = m_data.begin();
    m_data.erase(it + row, it + (row + count - 1));
    endRemoveRows();
    return true;
}

QVariant TrackModel::data(const QModelIndex &index, int role) const {
    qDebug() << "getting data" << roleNames()[role] << "at index" << index.row();
    int i = index.row();
    if (i >= m_data.size() || i < 0)
        return QVariant(-10);
    else {
        Track t = m_data[i];
        switch (role) {
        case VolRole:
            qDebug() << t.vol();
            return t.vol();
        case PanRole:
            qDebug() << t.pan();
            return t.pan();
        case OutRole:
            qDebug() << t.out();
            return qlonglong(t.out());
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
    qDebug() << "setting data" << roleNames()[role] << "at index" << index.row() << "to" << value;
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

    QVector<int> roles = QVector<int> ();
    roles.append(role);
    emit(QAbstractListModel::dataChanged(index, index, roles));
    return true;
}

void TrackModel::addTrack(const Track& t) {
    if (insertRows(rowCount(), 1)) {
        QModelIndex index = TrackModel::index(rowCount() - 1);
        qDebug() << "adding track at index" << index.row();
        setData(index, t.vol(), VolRole);
        setData(index, t.pan(), PanRole);
        setData(index, qlonglong(t.out()), OutRole);
        qDebug() << "added track";
    }
}

void TrackModel::removeTrack(int index) {
    removeRows(index, 1);
}

double TrackModel::getVol(int i) const {
    return data(index(i), VolRole).toDouble();
}

double TrackModel::getPan(int i) const {
    return data(index(i), PanRole).toDouble();
}

double TrackModel::getOut(int i) const {
    return data(index(i), OutRole).toDouble();
}

void TrackModel::setVol(int i, double volval) {
    setData(index(i), volval, VolRole);
}

void TrackModel::setPan(int i, double panval) {
    setData(index(i), panval, PanRole);
}

void TrackModel::setOut(int i, long outval) {
    setData(index(i), qlonglong(outval), OutRole);
}
