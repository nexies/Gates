import QtQuick
import QtQuick.Controls

GridView
{
    property string name: "ItemGridView"
    id: grid

    visible: true
    focus: true

    anchors.leftMargin: 20
    anchors.rightMargin: 20
    snapMode: GridView.SnapToRow
    boundsBehavior: GridView.StopAtBounds

    // ScrollBar.vertical: ScrollBar
    // {
    //     id: scrollBar
    //     rightPadding: -7
    // }

    highlight: Rectangle
    {
        radius: 5
        color: "white"
        opacity: 0.4
    }

    highlightMoveDuration: 0
    highlightFollowsCurrentItem: true

    cellHeight: 100
    cellWidth: 100


    delegate: GatesGridViewDelegate
    {
        MouseArea
        {
            anchors.fill: parent
            onClicked: { grid.currentIndex = index; }
            onDoubleClicked: {
                desktopService.openLocalFile(filePath);
            }
        }
    }
}
