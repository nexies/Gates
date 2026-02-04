import QtQuick
import QtQuick.Controls
import CustomQmlTypes


Item {
    property string name: "FrameNameBar"
    width: parent.width
    height: 40
    property alias text: nameLabel.text
    property bool frameMinimised: false

    id: viewport

    anchors.horizontalCenter: parent.horizontalCenter

    Text
    {
        id: nameLabel

        font
        {
            pixelSize: 20
            bold: true
            // family: "Manjari"
            family: "Tex Gyre Adventor"
        }
        color: "white"
        text: "Frame"

        antialiasing: true
        opacity: 1

        anchors.horizontalCenter: parent.horizontalCenter
        // anchors.verticalCenter: parent.verticalCenter
        anchors.top: parent.top
        anchors.topMargin: (parent.height - font.pixelSize) / 2 + 3
    }


    NameBarCustomButton
    {
        id: optionsButton
        iconImage: "qrc:/icons/more_horiz.png"
        width: 30
        height: 30

        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: (parent.height - height) / 2
    }

    NameBarCustomButton
    {
        id: minimizeButton
        iconImage: "qrc:/icons/chevron_left.png"
        width: 30
        height: 30

        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.rightMargin: (parent.height - height) / 2

        property int rotationAngle: frameMinimised ? -90 : 90
        Behavior on rotationAngle { NumberAnimation {easing.type: Easing.InOutQuad; duration: 200}}

        transform: Rotation
        {
            axis {x: 0; y: 0; z: 1}
            origin.x: 15
            origin.y:15
            angle: minimizeButton.rotationAngle
        }

        onButtonClicked: { minimiseButtonTriggered(); }
    }

    signal minimiseButtonTriggered()
    signal optionsButtonTriggered()
}
