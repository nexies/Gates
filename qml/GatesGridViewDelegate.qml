import QtQuick
import QtQuick.Controls


Item {
    id: delegate
    property string name: "GatesGridViewDelegate"

    width: 100
    height: 100

    Item{
        id: wrapper
        anchors.fill: parent


        Image {
            id: icon
            width: 70
            height: 70
            source: "image://gates_icon_provider/" + filePath
            anchors.horizontalCenter: wrapper.horizontalCenter
            anchors.top: wrapper.top
            anchors.topMargin: 5
            asynchronous: true
        }

        Text
        {
            id: iconText
            width: parent.width - 6
            anchors.top: icon.bottom
            anchors.bottom: parent.bottom

            anchors.horizontalCenter: wrapper.horizontalCenter
            anchors.topMargin: 3
            maximumLineCount: 3
            horizontalAlignment: Text.AlignHCenter
            text: fileName
            wrapMode: Text.WordWrap
            elide: Text.ElideRight
            color: "white"
        }
    }

}
