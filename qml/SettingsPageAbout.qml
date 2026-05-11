import QtQuick

Item {
    Column {
        anchors.centerIn: parent
        spacing: 0

        Image {
            source: "qrc:/icons/settings_filled.png"
            width: 56; height: 56
            anchors.horizontalCenter: parent.horizontalCenter
            opacity: 0.85
        }

        Item { width: 1; height: 18 }

        Text {
            text: "Gates"
            color: "white"; font.family: "Tex Gyre Adventor"
            font.pixelSize: 38; font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Item { width: 1; height: 6 }

        Text {
            text: "Версия 0.1.0"
            color: Qt.rgba(1,1,1,0.4); font.family: "Tex Gyre Adventor"; font.pixelSize: 13
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Item { width: 1; height: 24 }

        Rectangle {
            width: 220; height: 1; color: Qt.rgba(1,1,1,0.10)
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Item { width: 1; height: 24 }

        Text {
            text: "Менеджер рабочего стола для Windows\nв стиле Stardock Fences"
            color: Qt.rgba(1,1,1,0.55); font.family: "Tex Gyre Adventor"; font.pixelSize: 13
            horizontalAlignment: Text.AlignHCenter
            lineHeight: 1.5
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Item { width: 1; height: 16 }

        Text {
            text: "Построено на Qt 6 + QML"
            color: Qt.rgba(1,1,1,0.30); font.family: "Tex Gyre Adventor"; font.pixelSize: 12
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
