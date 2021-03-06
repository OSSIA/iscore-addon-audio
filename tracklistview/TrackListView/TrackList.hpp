#pragma once
#include <QObject>
#include <vector>
#include <QAbstractListModel>

class Track {
public:
    Track(double, double, long);
    Track();

    double vol() const;
    double pan() const;
    long out() const;

    void setVol(double);
    void setPan(double);
    void setOut(long);

private:
    double m_vol = 100;
    double m_pan = 0;
    long m_output = 0;
};

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

    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(int);
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

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
