import QtQuick 2.0

Item {
    id: circularButtonRoot

    property double value: 0
    property double minvalue: -1
    property double maxvalue: 1

    function calculate (mx, my) {
        var x = mx - width / 2;
        var y = my - height / 2;

        if (x == 0) {
            if (y > 0)
                return 0;
            else
                return value;
        }

        var a;
        if (x > 0) {
            a = Math.atan(y / x)
        }
        else {
            a = Math.PI - Math.atan(-y / x)
        }

        if (a > 2 * Math.PI / 3)
            a = 2 * Math.PI / 3
        if (a < -2 * Math.PI / 3)
            a = -2 * Math.PI / 3

        return (a * 180) / Math.PI
    }

    width: 40
    height: 40

    Image {
        id: img
        source: "qrc:/img/button.png"

        anchors.fill: parent

        Image {
            id: hand
            source: "qrc:/img/hand.png"

            anchors.fill: parent
            transform: Rotation {angle: value * 120; origin.x: width / 2; origin.y: height / 2 }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent

            preventStealing: true
            onPositionChanged: {
                var alpha = calculate(mouse.x, mouse.y)
                console.log(alpha)
                circularButtonRoot.value = alpha / 120
            }
        }
    }
}
