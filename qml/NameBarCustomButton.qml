import QtQuick
import QtQuick.Controls
import CustomQmlTypes

Item {
    property string name: "NameBarCustomButton"

    property url iconImage
    property int iconSize: width > height ? height : width
    property string displayedText
    property bool hovered: false
    property bool pressed: false

    Rectangle
    {
        id: highlight
        anchors.fill: parent
        color: "white"
        radius: 3
        border
        {
            width: 1
            color: "white"
        }
        opacity: { if(hovered) return 0.1; else return 0; }
        Behavior on opacity { NumberAnimation { easing.type: Easing.InOutQuad; duration: 100 }}
    }

    Image
    {
        id: image
        anchors.centerIn: parent
        width: iconSize; height: iconSize;
        source: iconImage
        Behavior on opacity { NumberAnimation { easing.type: Easing.InOutQuad; duration: 100 }}
        antialiasing: true
        opacity:
        {
            if(mouseArea.pressed)
                return 1
            if(hovered)
                return 0.5
            return 0.3
        }
    }

    MouseArea
    {
        id: mouseArea
        preventStealing: true
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton

        anchors.fill: parent
        // onPressed: { pressed = true; }
        onClicked: { buttonClicked() }

        // onReleased: { pressed = false; }

        onEntered: { hovered = true; }
        onExited:  { hovered = false; }
    }

    signal buttonClicked()
}
