import QtQuick
import QtQuick.Controls

Item {
    id: delegate

    width:  GridView.view ? GridView.view.cellWidth  : 100
    height: GridView.view ? GridView.view.cellHeight : 100

    // Dim the source icon while it is being dragged
    opacity: dragDropService.active &&
             dragDropService.dragPath      === filePath &&
             dragDropService.sourceFrameId === frameId ? 0.25 : 1.0
    Behavior on opacity { NumberAnimation { duration: 100 } }

    // ── Icon ──────────────────────────────────────────────────────────────────
    Image {
        id: icon
        anchors.top:              parent.top
        anchors.topMargin:        6
        anchors.horizontalCenter: parent.horizontalCenter
        width:  56
        height: 56
        source: "image://gates_icon_provider/" + filePath
        fillMode:    Image.PreserveAspectFit
        asynchronous: true
        smooth:       true
    }

    // ── Label ─────────────────────────────────────────────────────────────────
    Text {
        id: label
        anchors.top:              icon.bottom
        anchors.topMargin:        4
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

        style:      Text.Outline
        styleColor: "#99000000"
    }

    // ── Drag ──────────────────────────────────────────────────────────────────
    DragHandler {
        id: iconDrag
        target: null   // don't move the delegate — ghost window handles visuals
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
