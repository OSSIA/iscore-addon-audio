#pragma once
#include <QAbstractListModel>
#include <vector>
#include <QObject>
#include <QVariant>
#include "Track.hpp"

namespace Audio {
namespace Panel {

class TrackModel : public QAbstractListModel {
    Q_OBJECT
public:
    TrackModel(QObject* parent = 0);
    enum TrackRoles {
        VolRole = Qt::UserRole + 1,
        PanRole,
        OutRole
    };
    Q_ENUMS(TrackRoles)

    Q_INVOKABLE void addTrackSignal();

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(int);
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    Q_INVOKABLE double getVol(int index) const;
    Q_INVOKABLE double getPan(int index) const;
    Q_INVOKABLE long getOut(int index) const;

    Q_INVOKABLE void setVol(int index, double volval);
    Q_INVOKABLE void setPan(int index, double panval);
    Q_INVOKABLE void setOut(int index, long outval);

    Q_INVOKABLE void print();

public slots:
    void addTrack(const Track&);
    void removeTrack(int index);

private:
    std::vector<Track> m_data;
};

}}
