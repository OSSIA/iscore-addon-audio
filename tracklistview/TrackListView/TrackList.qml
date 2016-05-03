import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

Rectangle {
    id: trackRoot
    color: "darkGrey"
    ListView {
        id: trackListView
        orientation: Qt.Horizontal
        anchors.fill: parent
        spacing: 5

        property int itemheight
        onItemheightChanged: { addTrackButton.y = itemheight + 5; }

        delegate:
            Rectangle {
            id: wrapper
            height: columnWrap.height + 20
            width: removeButton.width + 20
            color: "lightgrey"
            border.color: "black"
            anchors.margins: 5

            Component.onCompleted: {
                trackListView.itemheight = columnWrap.height + 20;
            }

            ColumnLayout {
                id: columnWrap
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter

                CircularButton {
                    id: panButton
                    value: trackModel.getPan(index)
                    onValueChanged: trackModel.setPan(index, value)
                    anchors.horizontalCenter: parent.horizontalCenter
                }

//                Slider {
//                    id: panSlider
//                    orientation: Qt.Horizontal
//                    value: trackModel.getPan(index)
//                    minimumValue: -1
//                    maximumValue: 1
//                    width: width
//                    stepSize: 0.001
//                    onValueChanged: {trackModel.setPan(index, value); panButton.value = value}
//                }

                Label {
                    anchors.horizontalCenter: columnWrap.horizontalCenter
                    text: "Pan %1".arg(panButton.value)
                }

                Slider {
                    anchors.horizontalCenter: columnWrap.horizontalCenter
                    id: volSlider
                    tickmarksEnabled: true
                    orientation: Qt.Vertical
                    value: trackModel.getVol(index)
                    stepSize: 1
                    minimumValue: 0
                    maximumValue: 100
                    onValueChanged: trackModel.setVol(index, value)
                }
                Label {
                    text: "Volume " + volSlider.value
                    anchors.horizontalCenter: columnWrap.horizontalCenter
                    anchors.top: volSlider.bottom
                }

                SpinBox {
                    anchors.horizontalCenter: columnWrap.horizontalCenter
                    id: outBox
                    value: out
                    stepSize: 1
                    minimumValue: 0
                    onValueChanged: trackModel.setOut(index, value)
                }
                Label {
                    anchors.horizontalCenter: columnWrap.horizontalCenter
                    text: "Output"
                }

                Button {
                    id: removeButton
                    text: "Remove"
                    anchors.horizontalCenter: columnWrap.horizontalCenter
                    onClicked: trackModel.removeTrack(index)
                }
            }
        }

        model: trackModel
    }


    Button {
        id: addTrackButton
        text: "Add a track"
        onClicked: trackModel.addTrackSignal()
    }
}
