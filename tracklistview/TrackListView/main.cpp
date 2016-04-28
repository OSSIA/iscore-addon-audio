#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#include <QDebug>
#include <QUrl>

#include "DeviceInfo.hpp"
#include "TrackList.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    engine.load(QUrl(QStringLiteral("qrc:/Track.qml")));
    engine.load(QUrl(QStringLiteral("qrc:/TrackList.qml")));

    TrackModel tm;
    tm.roleNames();
    qDebug() << "Successfully created track model";
    tm.addTrack(Track(40, 0.6, 7));
    tm.addTrack(Track(70, -0.2, 3));
    tm.addTrack(Track(33, 0, 1));

    engine.rootContext()->setContextProperty("trackModel", &tm);

    return app.exec();
}
