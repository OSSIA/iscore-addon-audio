#pragma once
#include <QObject>
#include <vector>
#include <QAbstractListModel>

class Track {
public:
    Track();

    double volume();
    double pan();
    long output();

    void setVolume(double);
    void setPan(double);
    void setOutput(long);

private:
    double m_vol;
    double m_pan;
    long m_output;
};

class TrackList : public std::vector<Track> {

};

class TrackModel : public QObject {
    Q_OBJECT
public:
    TrackModel(QObject* parent = 0);
    enum TrackRoles {
        VolRole = Qt::UserRole + 1,
        PanRole,
        OutRole
    };

    QHash<int, QByteArray> roleNames();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(int);

public slots:
    void addTrack(Track, int index);
    void insertRows();

private:
    std::vector<Track> m_data;
};
