import QtQuick
import QtQuick.Controls

Item {
    id: delegate

    width:  GridView.view ? GridView.view.cellWidth  : 100
    height: GridView.view ? GridView.view.cellHeight : 100

    // Dim while this specific icon is being dragged
    opacity: dragDropService.active &&
             dragDropService.dragPath      === filePath &&
             dragDropService.sourceFrameId === frameId ? 0.25 : 1.0
    Behavior on opacity { NumberAnimation { duration: 100 } }

    // True when this item is selected AND the parent view has keyboard focus
    readonly property bool _selected: GridView.isCurrentItem &&
                                      GridView.view   !== null &&
                                      GridView.view.activeFocus

    // True while the user is renaming this item (set by IconGridView via currentItemRenaming)
    readonly property bool _renaming: GridView.isCurrentItem &&
                                      GridView.view !== null &&
                                      GridView.view.currentItemRenaming

    // ── Hover / press / selection highlight ──────────────────────────────────
    Rectangle {
        anchors.fill:    parent
        anchors.margins: 2
        radius:  4
        color:   "white"
        opacity: pressDetector.pressed ? 0.28
               : _selected             ? 0.22
               : hoverHandler.hovered  ? 0.13
               : 0.0
        Behavior on opacity { NumberAnimation { duration: 80 } }
    }

    // ── Icon ─────────────────────────────────────────────────────────────────
    Image {
        id: icon
        anchors.top:              parent.top
        anchors.topMargin:        8
        anchors.horizontalCenter: parent.horizontalCenter
        width:  48
        height: 48
        source: "image://gates_icon_provider/" + filePath
        fillMode:     Image.PreserveAspectFit
        asynchronous: true
        smooth:       true
    }

    // ── Label (normal) ───────────────────────────────────────────────────────
    Text {
        id: labelText
        visible: !_renaming
        anchors.top:              icon.bottom
        anchors.topMargin:        3
        anchors.bottom:           parent.bottom
        anchors.bottomMargin:     4
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 8

        text:               fileName
        color:              "white"
        font.pixelSize:     11
        horizontalAlignment: Text.AlignHCenter
        wrapMode:           Text.WordWrap
        maximumLineCount:   2
        elide:              Text.ElideRight
        style:              Text.Outline
        styleColor:         "#99000000"
    }

    // ── Label (rename mode) ──────────────────────────────────────────────────
    Rectangle {
        id: renameBox
        visible: _renaming
        anchors.top:              icon.bottom
        anchors.topMargin:        2
        anchors.horizontalCenter: parent.horizontalCenter
        width:  parent.width - 4
        height: renameInput.implicitHeight + 4
        color:  "#1a6fc4"
        radius: 3

        TextInput {
            id: renameInput
            anchors.fill:        parent
            anchors.margins:     2
            color:               "white"
            font.pixelSize:      11
            horizontalAlignment: TextInput.AlignHCenter
            selectByMouse:       true
            clip:                true

            onVisibleChanged: {
                if (visible) {
                    text = fileName
                    forceActiveFocus()
                    // Select base name only (without extension)
                    const dot = text.lastIndexOf('.')
                    select(0, dot > 0 ? dot : text.length)
                }
            }

            Keys.onReturnPressed: commit()
            Keys.onEnterPressed:  commit()
            Keys.onEscapePressed: cancel()

            function commit() {
                desktopService.renameFile(filePath, text)
                GridView.view.currentItemRenaming = false
                GridView.view.forceActiveFocus()
            }
            function cancel() {
                GridView.view.currentItemRenaming = false
                GridView.view.forceActiveFocus()
            }
        }
    }

    // ── Hover detection ──────────────────────────────────────────────────────
    HoverHandler { id: hoverHandler }

    // ── Press detection (for immediate visual feedback) ───────────────────────
    TapHandler { id: pressDetector; acceptedButtons: Qt.LeftButton }

    // ── Drag ─────────────────────────────────────────────────────────────────
    DragHandler {
        id: iconDrag
        target: null
        dragThreshold: 6
        grabPermissions: PointerHandler.CanTakeOverFromItems |
                         PointerHandler.CanTakeOverFromHandlersOfDifferentType

        onActiveChanged: {
            const gp = delegate.mapToGlobal(centroid.position.x, centroid.position.y)
            if (active)
                dragDropService.startDrag(frameId, filePath, gp.x, gp.y)
            else
                dragDropService.commitDrop(gp.x, gp.y)
        }

        onCentroidChanged: {
            if (active) {
                const gp = delegate.mapToGlobal(centroid.position.x, centroid.position.y)
                dragDropService.updatePos(gp.x, gp.y)
            }
        }
    }
}
