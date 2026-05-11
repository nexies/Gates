import QtQuick
import QtQuick.Controls

GridView {
    id: grid

    property bool currentItemRenaming: false

    visible: true
    focus:   true

    anchors.leftMargin:  desktopConfig.snapFramesToGrid ? desktopConfig.gapX : 20
    anchors.rightMargin: desktopConfig.snapFramesToGrid ? desktopConfig.gapX : 20
    clip:                true
    snapMode:            GridView.SnapToRow
    boundsBehavior:      GridView.StopAtBounds

    highlight:                   null
    highlightMoveDuration:       0
    highlightFollowsCurrentItem: true

    cellWidth:  desktopConfig.snapFramesToGrid ? desktopConfig.stepX : 100
    cellHeight: desktopConfig.snapFramesToGrid ? desktopConfig.stepY : 100

    onCurrentIndexChanged: currentItemRenaming = false

    // Deselect globally when this frame loses OS-window focus
    onActiveFocusChanged: if (!activeFocus) selectionService.deselect()

    property real _savedContentY: 0
    Connections {
        target: grid.model
        function onModelAboutToBeReset() { grid._savedContentY = grid.contentY }
        function onModelReset() {
            Qt.callLater(function() { grid.contentY = grid._savedContentY })
        }
    }

    Keys.onPressed: (event) => {
        if (event.key === Qt.Key_F2 && currentIndex >= 0 && !currentItemRenaming) {
            currentItemRenaming = true
            event.accepted = true
        }
    }

    delegate: IconDelegate {
        iconPath:    filePath
        iconName:    fileName
        iconFrameId: frameId
        isRenaming:  GridView.isCurrentItem && grid.currentItemRenaming

        onSelectRequested: {
            grid.currentIndex = index
            selectionService.select(filePath)
            grid.forceActiveFocus()
        }
        onOpenRequested:   desktopService.shellOpen(filePath)
        onContextMenuRequested: (gx, gy) => {
            grid.currentIndex = index
            selectionService.select(filePath)
            desktopService.showContextMenu(filePath, gx, gy)
        }
    }
}
