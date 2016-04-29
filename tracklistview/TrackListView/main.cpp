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

    TrackModel tm;
    tm.addTrack(Track(40, 0.7, 12));
    tm.addTrack(Track(33, -0.3, 2));
    for (int i = 0; i < 12; ++i)
        tm.addTrack(Track());
    tm.addTrack(Track(25, 0, 3));

    tm.print();

    engine.rootContext()->setContextProperty("trackModel", &tm);

    engine.load(QUrl(QStringLiteral("qrc:/TrackList.qml")));

    tm.print();

    return app.exec();
}
