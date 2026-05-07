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

            // ── Drag to reposition ───────────────────────────────────────────
            DragHandler {
                onActiveChanged: {
                    if (!active)
                        desktopIconModel.setPosition(model.index,
                                                     iconDelegate.x,
                                                     iconDelegate.y)
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
