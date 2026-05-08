import QtQuick
import QtQuick.Controls

Item {
    id: delegate

    // ── Required properties (set by parent) ───────────────────────────────────
    property string iconPath:    ""   // absolute file path (image source + rename target)
    property string iconName:    ""   // display label
    property string iconFrameId: ""   // "" for desktop, frame UUID for frames

    // Set to true by parent when this item should show a rename input.
    // Only meaningful for frame icons (desktop window has no keyboard focus).
    property bool   isRenaming:  false

    // ── Signals (handled by parent) ───────────────────────────────────────────
    signal selectRequested()
    signal openRequested()
    signal contextMenuRequested(real globalX, real globalY)
    signal dragDropped(bool droppedOnFrame, real globalX, real globalY)

    // ── Size: defer to GridView cell when inside one, else use properties ─────
    width:  GridView.view ? GridView.view.cellWidth  : width
    height: GridView.view ? GridView.view.cellHeight : height

    // ── Drag-ghost dim: fade out source icon while dragging ───────────────────
    opacity: dragDropService.active        &&
             dragDropService.dragPath      === iconPath &&
             dragDropService.sourceFrameId === iconFrameId ? 0.25 : 1.0
    Behavior on opacity { NumberAnimation { duration: 100 } }

    // ── Hover / press / selection highlight ───────────────────────────────────
    Rectangle {
        anchors.fill:    parent
        anchors.margins: 2
        radius:  4
        color:   "white"
        opacity: pressDetector.pressed                       ? 0.28
               : selectionService.selectedPath === iconPath  ? 0.22
               : hoverHandler.hovered                        ? 0.13
               : 0.0
        Behavior on opacity { NumberAnimation { duration: 80 } }
    }

    // ── Icon image ────────────────────────────────────────────────────────────
    Image {
        id: iconImage
        anchors.top:              parent.top
        anchors.topMargin:        8
        anchors.horizontalCenter: parent.horizontalCenter
        width:  48
        height: 48
        source: iconPath ? "image://gates_icon_provider/" + iconPath : ""
        fillMode:     Image.PreserveAspectFit
        asynchronous: true
        smooth:       true
    }

    // ── Label (normal) ────────────────────────────────────────────────────────
    Text {
        visible: !isRenaming
        anchors.top:              iconImage.bottom
        anchors.topMargin:        3
        anchors.bottom:           parent.bottom
        anchors.bottomMargin:     4
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 8

        text:               iconName
        color:              "white"
        font.pixelSize:     11
        horizontalAlignment: Text.AlignHCenter
        wrapMode:           Text.WordWrap
        maximumLineCount:   2
        elide:              Text.ElideRight
        style:              Text.Outline
        styleColor:         "#99000000"
    }

    // ── Rename input ──────────────────────────────────────────────────────────
    Rectangle {
        visible: isRenaming
        anchors.top:              iconImage.bottom
        anchors.topMargin:        2
        anchors.horizontalCenter: parent.horizontalCenter
        width:  parent.width - 4
        height: renameInput.implicitHeight + 4
        color:  "#1a6fc4"
        radius: 3

        TextInput {
            id: renameInput
            anchors { fill: parent; margins: 2 }
            color:               "white"
            font.pixelSize:      11
            horizontalAlignment: TextInput.AlignHCenter
            selectByMouse:       true
            clip:                true

            onVisibleChanged: {
                if (visible) {
                    text = iconName
                    forceActiveFocus()
                    const dot = text.lastIndexOf('.')
                    select(0, dot > 0 ? dot : text.length)
                }
            }

            Keys.onReturnPressed: _commit()
            Keys.onEnterPressed:  _commit()
            Keys.onEscapePressed: _cancel()

            function _commit() {
                desktopService.renameFile(iconPath, text)
                _endRename()
            }
            function _cancel() { _endRename() }
            function _endRename() {
                if (GridView.view) {
                    GridView.view.currentItemRenaming = false
                    GridView.view.forceActiveFocus()
                }
            }
        }
    }

    // ── Tooltip ───────────────────────────────────────────────────────────────
    ToolTip {
        visible: hoverHandler.hovered && !iconDrag.active && !isRenaming
        delay:   700
        text:    iconPath.startsWith("::virtual::") ? iconName : iconPath
    }

    // ── Input handlers ────────────────────────────────────────────────────────
    HoverHandler { id: hoverHandler }

    TapHandler {
        id: pressDetector
        acceptedButtons: Qt.LeftButton
        onTapped:       delegate.selectRequested()
        onDoubleTapped: delegate.openRequested()
    }

    TapHandler {
        acceptedButtons: Qt.RightButton
        onTapped: (ev) => delegate.contextMenuRequested(
            Math.round(ev.globalPosition.x),
            Math.round(ev.globalPosition.y))
    }

    DragHandler {
        id: iconDrag
        target:        null
        dragThreshold: 6
        grabPermissions: PointerHandler.CanTakeOverFromItems |
                         PointerHandler.CanTakeOverFromHandlersOfDifferentType

        onActiveChanged: {
            const gp = delegate.mapToGlobal(centroid.position.x, centroid.position.y)
            if (active) {
                dragDropService.startDrag(iconFrameId, iconPath, gp.x, gp.y)
            } else {
                const dropped = dragDropService.commitDrop(gp.x, gp.y)
                delegate.dragDropped(dropped, gp.x, gp.y)
            }
        }
        onCentroidChanged: {
            if (active) {
                const gp = delegate.mapToGlobal(centroid.position.x, centroid.position.y)
                dragDropService.updatePos(gp.x, gp.y)
            }
        }
    }
}
