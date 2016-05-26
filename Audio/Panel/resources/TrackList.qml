import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Extras 1.4
import QtQuick.Controls.Styles 1.4

Rectangle {
    id: trackRoot

    width: trackListView.width
    height: trackListView.height + 50

    ListView {
        id: trackListView
        orientation: Qt.Horizontal
        anchors.fill: parent
        spacing: 5

        property int itemheight
        onItemheightChanged: { addTrackButton.y = itemheight + 5; }
        width: 800
        height: 100

        delegate:
            Rectangle {
            id: wrapper
            height: columnWrap.height + 20
            width: removeButton.width + 50
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

                Rectangle {
                    color: "transparent"

                    width: panButton.width
                    height: panButton.height + 50
                    anchors.horizontalCenter: parent.horizontalCenter

                    Dial {
                        anchors.verticalCenter: parent.verticalCenter
                        id: panButton
                        width: 70
                        height: 70
                        anchors.verticalCenterOffset: 25
                        anchors.horizontalCenterOffset: 0
                        maximumValue: 1
                        minimumValue: -1
                        tickmarksVisible: true
                        stepSize: 0.01
                        style: DialStyle {
                            background: Image {
                                source: "img/button.png"
                                width: 30
                                height: 30
                            }
                            tickmarkStepSize: 0.5
                            tickmarkInset: 2
                            tickmark: Rectangle {
                                color: "#FF0000"
                                implicitHeight: 7
                                implicitWidth: 3
                                radius: width / 2.;
                            }

                            minorTickmark: Rectangle {
                                color: "#FFFFFF"
                                implicitHeight: 3
                                implicitWidth: 3
                                radius: width / 2
                            }
                            minorTickmarkCount: 4
                            minorTickmarkInset: 2

                            labelStepSize: 0.5
                            labelInset: -15
                            tickmarkLabel: Text {
                                color: "black"
                                text: styleData.value
                                font.pixelSize: 12
                            }
                        }
                        value: trackModel.getPan(index)
                        onValueChanged: {
                            trackModel.setPan(index, value)
                        }
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
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
                    onClicked: {
                        console.log("removing track " + index)
                        trackModel.removeTrack(index)
                    }
                }
            }
        }

        model: trackModel
    }

    Button {
        id: addTrackButton
        text: "Add a track"
        onClicked: {
            console.log ("adding track at index " + trackListView.count)
            console.log(trackModel)
            trackModel.addTrackSignal()
        }
    }
}
