#pragma once

#include <iscore/command/Dispatchers/CommandDispatcher.hpp>
#include <iscore/tools/IdentifiedObject.hpp>

#include <QAbstractListModel>
#include <QObject>
#include <QVariant>

#include <Audio/Panel/Track.hpp>
#include <vector>
#include <iscore/widgets/QmlContainerPanel.hpp>

namespace iscore {
    class DocumentContext;
}

namespace Audio {
namespace Panel {

class TrackModel : public QAbstractListModel {
    Q_OBJECT
public:
    TrackModel(const iscore::DocumentContext& ctx, QObject* parent);
    enum TrackRoles {
        VolRole = Qt::UserRole + 1,
        PanRole,
        OutRole
    };
    Q_ENUMS(TrackRoles)

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex&) const override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    Q_INVOKABLE double getVol(int index) const;
    Q_INVOKABLE double getPan(int index) const;
    Q_INVOKABLE long getOut(int index) const;

    Q_INVOKABLE void setVol(int index, double volval);
    Q_INVOKABLE void setPan(int index, double panval);
    Q_INVOKABLE void setOut(int index, int outval);

    Q_INVOKABLE void print();

    QMLContainerPanel* m_containerpanel {};
signals:
    void sig_addTrack();

public slots:
    void addTrack();
    void removeTrack(int index);

private:
    std::vector<Track> m_data;
    CommandDispatcher<> m_commandDispatcher;

};

}}