#include "TrackModel.hpp"
#include "Track.hpp"
#include <QVariant>
#include <QDebug>
#include <iscore/command/Dispatchers/CommandDispatcher.hpp>
#include <iscore/document/DocumentContext.hpp>
#include <Audio/Commands/AddTrack.hpp>
#include <Audio/Commands/RemoveTrack.hpp>
#include <Audio/Commands/SetData.hpp>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWidget>
namespace Audio {
namespace Panel {

TrackModel::TrackModel(const iscore::DocumentContext& ctx, QObject *parent) :
    QAbstractListModel(parent),
    m_commandDispatcher{ctx.commandStack}
{
    setObjectName("TrackModel");
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    connect(this, &TrackModel::sig_addTrack,
            this, &TrackModel::addTrack, Qt::QueuedConnection);



    m_containerpanel = new QMLContainerPanel{};

    QQuickWidget* container = m_containerpanel->container();
    QQmlEngine* engine = container->engine();
    QQmlContext* rootctxt = engine->rootContext();
    rootctxt->setContextProperty(QString("trackModel"), this);

    m_containerpanel->setSource(QString("qrc:/qml/TrackList.qml"));
    m_containerpanel->setContainerSize(m_containerpanel->size());
    m_containerpanel->setObjectName("TrackList");

}

QHash<int, QByteArray> TrackModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[VolRole] = "vol";
    roles[PanRole] = "pan";
    roles[OutRole] = "out";
    return roles;
}

bool TrackModel::insertRows(int row, int count, const QModelIndex& parent) {
    if (row > m_data.size() || count <= 0)
        return false;

    beginInsertRows(parent, row, row + count - 1);
    m_data.insert(m_data.begin() + row, count, Track());
    endInsertRows();
    return true;
}

bool TrackModel::removeRows(int row, int count, const QModelIndex &parent) {
    if (row >= m_data.size()
            || row + count - 1 >= m_data.size()
            || row < 0
            || count < 0)
        return false;

    qDebug () << "sending beginRemoveRows" << parent << row << (row + count -1) << "...";

    beginRemoveRows(parent, row, row + count - 1);
    qDebug() << "sent.";
    auto it = m_data.begin();
    qDebug() << "got beginning iterator";
    m_data.erase(it + row, it + row + count);
    qDebug() << "erased rows from beginning +" << row << "to beginning +" << (row + count);
    endRemoveRows();
    qDebug() << "sent endRemoveRows signal";
    return true;
}

QVariant TrackModel::data(const QModelIndex &index, int role) const {
    int i = index.row();
    if (i < m_data.size() && i >= 0) {
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
    return QVariant(-10);
}

int TrackModel::rowCount(const QModelIndex& m) const {
    return m_data.size();
}

Qt::ItemFlags TrackModel::flags(const QModelIndex&) const {
    return Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool TrackModel::setData(const QModelIndex &index, const QVariant &value, int role) {

    qDebug() << "setting value" << roleNames()[role] << "of index" << index.row() << "to" << value;
    int i = index.row();
    if (i < 0)
        return false;

    switch (role) {
    case VolRole:
        m_data[i].setVol(value.toDouble());
        break;
    case PanRole:
        m_data[i].setPan(value.toDouble());
        break;
    case OutRole:
        m_data[i].setOut(value.toInt());
        break;
     default:
        return false;
    }

    qDebug() << "set value done.";

    QVector<int> roles = QVector<int> (1, role);
    emit(QAbstractListModel::dataChanged(index, index, roles));

    qDebug() << "sent 'data changed' signal";

    return true;
}

void TrackModel::addTrack() {
    qDebug() << "submitting command for new AddTrack with index" << rowCount();
    m_commandDispatcher.submitCommand(new Commands::AddTrack{*this, Track{}});
}

void TrackModel::removeTrack(int index) {
    qDebug() << "submitting command for new RemoveTrack with index" << index;
    m_commandDispatcher.submitCommand(new Commands::RemoveTrack{*this, index});
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
    m_commandDispatcher.submitCommand(new Commands::SetData{*this, i, volval, VolRole});
}

void TrackModel::setPan(int i, double panval) {
    m_commandDispatcher.submitCommand(new Commands::SetData{
                                          *this, i, panval, PanRole
                                      });
}

void TrackModel::setOut(int i, int32_t outval) {
    m_commandDispatcher.submitCommand(new Commands::SetData{
                                          *this, i, outval, OutRole
                                      });
}

void TrackModel::print() {
    int s = m_data.size();
    for (int i = 0; i < s; ++i) {
        Track t = m_data[i];
        qDebug() << "Track" << i << "(" << t.pan() << t.vol() << t.out() << ")";
    }
}


}
}
