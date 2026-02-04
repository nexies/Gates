import QtQuick

Window {
    width: 300
    height: 300

    flags: Qt.Widget | Qt.FramelessWindowHint
    color: "transparent"
    visible: true

    Rectangle
    {
        anchors.fill: parent
        opacity: 0.6
        color: "black"
    }
}
