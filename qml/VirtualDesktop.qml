import QtQuick
import QtQuick.Controls
import Gates

Window {
    id: root

    flags:  Qt.Window | Qt.FramelessWindowHint
    color:  "transparent"
    visible: true

    // Tap on empty desktop area → deselect all
    // _blockDeselect is set by icon tap to prevent this handler from clearing the
    // selection that was just applied (both handlers fire for the same event).
    property bool _blockDeselect: false

    TapHandler {
        acceptedButtons: Qt.LeftButton
        onTapped: {
            if (root._blockDeselect) { root._blockDeselect = false; return }
            selectionService.deselect()
        }
    }

    TapHandler {
        acceptedButtons: Qt.RightButton
        onTapped: (eventPoint) => desktopService.showDesktopContextMenu(
            Math.round(eventPoint.globalPosition.x),
            Math.round(eventPoint.globalPosition.y))
    }

    // ── Grid layout — live-bound to desktopConfig context property ──────────
    readonly property int cellW:  desktopConfig.cellW
    readonly property int cellH:  desktopConfig.cellH
    readonly property int gapX:   desktopConfig.gapX
    readonly property int gapY:   desktopConfig.gapY
    readonly property int margin: desktopConfig.margin
    readonly property int stepX:  desktopConfig.stepX
    readonly property int stepY:  desktopConfig.stepY

    // ── Debug: grid overlay ──────────────────────────────────────────────────
    Canvas {
        id: debugGrid
        anchors.fill: parent
        visible: desktopConfig.showDebugGrid

        Connections {
            target: desktopConfig
            function onLayoutChanged() { debugGrid.requestPaint() }
        }

        onPaint: {
            const ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            ctx.strokeStyle = "rgba(255,0,0,0.4)"
            ctx.lineWidth   = 1
            const cols = Math.floor((width  - 2 * root.margin) / root.stepX)
            const rows = Math.floor((height - 2 * root.margin) / root.stepY)
            for (let c = 0; c <= cols; c++) {
                const x = root.margin + c * root.stepX
                ctx.beginPath(); ctx.moveTo(x, root.margin); ctx.lineTo(x, root.margin + rows * root.stepY); ctx.stroke()
            }
            for (let r = 0; r <= rows; r++) {
                const y = root.margin + r * root.stepY
                ctx.beginPath(); ctx.moveTo(root.margin, y); ctx.lineTo(root.margin + cols * root.stepX, y); ctx.stroke()
            }
        }
    }

    Repeater {
        model: desktopIconModel

        delegate: IconDelegate {
            iconPath:    model.path
            iconName:    model.displayName
            iconFrameId: ""
            width:  root.cellW
            height: root.cellH
            x:      model.iconX
            y:      model.iconY

            onSelectRequested: { root._blockDeselect = true; selectionService.select(model.path) }
            onOpenRequested:   desktopService.shellOpen(model.path)
            onContextMenuRequested: (gx, gy) =>
                desktopService.showContextMenu(model.path, gx, gy)

            onDragDropped: (droppedOnFrame, gx, gy) => {
                if (!droppedOnFrame) {
                    const localX  = gx - root.x
                    const localY  = gy - root.y
                    const maxCols = Math.floor((root.width  - 2 * root.margin) / root.stepX)
                    const maxRows = Math.floor((root.height - 2 * root.margin) / root.stepY)
                    const col = Math.max(0, Math.min(Math.floor((localX - root.margin) / root.stepX), maxCols - 1))
                    const row = Math.max(0, Math.min(Math.floor((localY - root.margin) / root.stepY), maxRows - 1))
                    desktopIconModel.setPosition(model.index,
                                                 root.margin + col * root.stepX,
                                                 root.margin + row * root.stepY)
                }
            }
        }
    }
}
