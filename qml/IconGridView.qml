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
    clip: true
    snapMode: GridView.SnapToRow
    boundsBehavior: GridView.StopAtBounds

    property real _savedContentY: 0
    Connections {
        target: grid.model
        function onModelAboutToBeReset() { grid._savedContentY = grid.contentY }
        function onModelReset()          { grid.contentY = grid._savedContentY }
    }

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
        TapHandler {
            acceptedButtons: Qt.LeftButton
            onTapped:        grid.currentIndex = index
            onDoubleTapped:  desktopService.shellOpen(filePath)
        }

        TapHandler {
            acceptedButtons: Qt.RightButton
            onTapped: (eventPoint) => {
                grid.currentIndex = index
                desktopService.showContextMenu(
                    filePath,
                    Math.round(eventPoint.globalPosition.x),
                    Math.round(eventPoint.globalPosition.y))
            }
        }
    }
}
