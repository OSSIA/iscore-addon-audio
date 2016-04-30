import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

Item {
    width: 640
    height: 480


    RowLayout {
        anchors.centerIn: parent
    }

    Track {
        id: track1
        x: 0
        y: 0
    }
}
