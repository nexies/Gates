import QtQuick
import QtQuick.Controls
import Gates

Window {
    id: root

    flags:  Qt.Window | Qt.FramelessWindowHint
    color:  "transparent"
    visible: true

    Repeater {
        model: desktopIconModel

        delegate: Item {
            id: iconDelegate

            width:  72
            height: 88
            x:      model.iconX
            y:      model.iconY

            // ── Icon ─────────────────────────────────────────────────────────
            Image {
                id: iconImage
                anchors.horizontalCenter: parent.horizontalCenter
                width:  48
                height: 48
                // Virtual items get their icon via the ::virtual:: prefix path —
                // IconProvider resolves the CLSID fallback on the C++ side
                source: "image://gates_icon_provider/" + model.path
                fillMode: Image.PreserveAspectFit
                smooth:   true
            }

            // ── Label ────────────────────────────────────────────────────────
            Text {
                anchors.top:              iconImage.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                width:   parent.width
                text:    model.displayName
                color:   "white"
                font.pixelSize: 12
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                maximumLineCount: 2
                elide: Text.ElideRight
                style:      Text.Outline
                styleColor: "#80000000"
            }

            // ── Drag to reposition / move to frame ──────────────────────────
            DragHandler {
                target: null   // ghost handles visuals; position updated on drop
                dragThreshold: 6

                onActiveChanged: {
                    const gp = iconDelegate.mapToGlobal(centroid.position.x,
                                                        centroid.position.y)
                    if (active) {
                        // sourceFrameId "" → came from desktop
                        dragDropService.startDrag("", model.path, gp.x, gp.y)
                    } else {
                        const droppedOnFrame = dragDropService.commitDrop(gp.x, gp.y)
                        if (!droppedOnFrame) {
                            // Landed on desktop — snap to grid and save position.
                            // root.x/y is the screen origin (set by DesktopLayer).
                            const cellW = 90
                            const cellH = 100
                            const localX = gp.x - root.x
                            const localY = gp.y - root.y
                            const snappedX = Math.round(localX / cellW) * cellW
                            const snappedY = Math.round(localY / cellH) * cellH
                            desktopIconModel.setPosition(model.index, snappedX, snappedY)
                        }
                        // else: FrameDispatcher.onDropOnFrame moves the file
                    }
                }

                onCentroidChanged: {
                    if (active) {
                        const gp = iconDelegate.mapToGlobal(centroid.position.x,
                                                            centroid.position.y)
                        dragDropService.updatePos(gp.x, gp.y)
                    }
                }
            }

            // ── Open on double-click ─────────────────────────────────────────
            TapHandler {
                acceptedButtons: Qt.LeftButton
                onDoubleTapped: desktopService.shellOpen(model.path)
            }

            // ── Native Shell context menu on right-click ──────────────────────
            TapHandler {
                acceptedButtons: Qt.RightButton
                onTapped: (eventPoint) => {
                    desktopService.showContextMenu(
                        model.path,
                        Math.round(eventPoint.globalPosition.x),
                        Math.round(eventPoint.globalPosition.y))
                }
            }
        }
    }
}
