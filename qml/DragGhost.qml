import QtQuick

// Transparent topmost window that follows the cursor during a drag operation.
// Qt.WindowTransparentForInput (WS_EX_TRANSPARENT) ensures mouse events pass
// through to the frame windows underneath.
Window {
    id: ghostWindow

    flags:   Qt.Tool | Qt.FramelessWindowHint |
             Qt.WindowStaysOnTopHint | Qt.WindowTransparentForInput
    color:   "transparent"
    width:   56
    height:  56
    visible: dragDropService.active

    x: dragDropService.ghostX - width  / 2
    y: dragDropService.ghostY - height / 2

    Image {
        anchors.fill: parent
        source: ghostWindow.visible
                ? ("image://gates_icon_provider/" + dragDropService.dragPath)
                : ""
        fillMode: Image.PreserveAspectFit
        smooth:   true
        opacity:  0.85
    }
}
