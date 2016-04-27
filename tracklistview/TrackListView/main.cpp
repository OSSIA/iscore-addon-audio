#include <QApplication>
#include <QQmlApplicationEngine>

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

    return app.exec();
}
