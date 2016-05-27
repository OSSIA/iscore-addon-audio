import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Extras 1.4
import QtQuick.Controls.Styles 1.4

Rectangle {
    id: trackRoot

    width: 0
    height: 0
    color: "#12171A"

    ListView {
        id: trackListView
        orientation: Qt.Horizontal
        anchors.fill: parent
        spacing: 5

        property int itemheight
        onItemheightChanged: { addTrackButton.y = itemheight + 5; }

        width: 800
        height: 100

        onCountChanged: {
            if (count > 0) {
                trackRoot.width = contentWidth + 500
                trackRoot.height = contentHeight + 1000
            }
        }

        delegate:
            Rectangle {
            id: wrapper
            height: columnWrap.height + 20
            width: removeButton.width + 30
            color: "#1A2024"
            border.color: "#666666"
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
                    height: panButton.height + 10
                    anchors.horizontalCenter: parent.horizontalCenter

                    Dial {
                        anchors.verticalCenter: parent.verticalCenter
                        id: panButton
                        anchors.verticalCenterOffset: 0
                        anchors.horizontalCenterOffset: 0
                        width: 50
                        height: 50
                        maximumValue: 1
                        minimumValue: -1
                        tickmarksVisible: true
                        stepSize: 0.01
                        style: DialStyle {
                            background: Image {
                                source: "img/button.png"
                            }
                            tickmarkStepSize: 0.5
                            tickmarkInset: 2
                            tickmark: Rectangle {
                                color: "#FFA200"
                                implicitHeight: 4
                                implicitWidth: 4
                                radius: width / 2.;
                            }

                            minorTickmark: Rectangle {
                                color: "silver"
                                implicitHeight: 3
                                implicitWidth: 3
                                radius: width / 2
                            }
                            minorTickmarkCount: 4
                            minorTickmarkInset: 2

//                            labelStepSize: 0.5
//                            labelInset: -10
//                            tickmarkLabel: Text {
//                                color: "black"
//                                text: styleData.value
//                                font.pixelSize: 8
//                            }
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
                    color: "silver"
                    horizontalAlignment: Text.AlignHCenter
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
                    horizontalAlignment: Text.horizontalAlignment
                    color: "silver"
                }

                SpinBox {
                    anchors.horizontalCenter: columnWrap.horizontalCenter
                    id: outBox
                    value: out
                    stepSize: 1
                    minimumValue: 0
                    onValueChanged: trackModel.setOut(index, value)
                    style: SpinBoxStyle {
                        background: Rectangle {
                            border.color: outBox.hovered ? "white" : "transparent"
                            border.width: 1
                            color: "#12171A"
                            radius: 2
                            implicitHeight: 20
                            implicitWidth: wrapper.width - 10
                        }
                        textColor: outBox.hovered ? "white" : "silver"
                    }
                }
                Label {
                    anchors.horizontalCenter: columnWrap.horizontalCenter
                    text: "Output"
                    color: "white"
                }

                Button {
                    id: removeButton
                    text: "Remove"
                    anchors.horizontalCenter: columnWrap.horizontalCenter
                    onClicked: {
                        console.log("removing track " + index)
                        trackModel.removeTrack(index)
                    }
                    style: ButtonStyle {
                        background: Rectangle {
                            color: "#1A2024"
                            radius: 2
                            border.color: removeButton.hovered ? "white" : "silver"
                        }
                        label: Text {
                            color: removeButton.hovered ? "white" : "silver"
                            anchors.centerIn: parent.Center
                            text: control.text
                            renderType: Text.NativeRendering
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }
            }
        }

        model: trackModel
    }

    Button {
        id: addTrackButton
        text: "Add a track"
        style: ButtonStyle {
            background: Rectangle {
                color: "#1A2024"
                radius: 2
                border.color: addTrackButton.hovered ? "white" : "silver"
            }
            label: Text {
                color: addTrackButton.hovered ? "white" : "silver"
                anchors.centerIn: parent.Center
                text: control.text
                renderType: Text.NativeRendering
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
        }
        onClicked: {
            trackModel.sig_addTrack()
            trackListView.positionViewAtEnd()
        }
    }
}
