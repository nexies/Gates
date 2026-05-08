import QtQuick
import QtQuick.Controls
import QtQuick.Window
import Gates

ResizableFramelessWindow {
    id: frameWindow

    title:   qsTr("Gates Frame")
    visible: true
    color:   "transparent"

    width:         600
    height:        500
    minimumHeight: namebar.height + 100
    minimumWidth:  140  // one icon column (100) + grid side margins (20+20)

    blurBehind: true

    resizeSnapToGrid: true
    sizeIncrement: Qt.size(100, 0)  // snap width to icon-column steps; 0 = free height
    baseSize:      Qt.size(140, 0)  // one column + left/right grid margins

    // ── State ─────────────────────────────────────────────────────────────────
    property int  nameBarPosition: GatesFrameState.NameBarOnTop
    property int  dockedState:     GatesFrameState.NotDocked
    property bool minimised:       false
    property int  maximisedHeight: 500

    // ── Style ─────────────────────────────────────────────────────────────────
    property alias frameColor: backgroundSettings.color
    // frameOpacity is stored but applied separately; aliasing it directly to
    // backgroundSettings.opacity would make nameBarBackground fully opaque at
    // typical config values (0.72 + 0.3 > 1.0).
    property double frameOpacity: 0.2

    QtObject {
        id: backgroundSettings
        property color  color:   "#1a1a2e"
        property int    radius:  0
        property double opacity: frameWindow.frameOpacity
    }

    // ── Collapse / expand animations ──────────────────────────────────────────
    // For bottom-docked frames the bottom edge must stay fixed: animate y and
    // height in parallel so that y + height = screen-bottom throughout.

    SequentialAnimation {
        id: minimiseAnimation
        PropertyAction { target: frameWindow; property: "minimumHeight"; value: namebar.height }
        ScriptAction {
            script: {
                collapseYAnim.to = (frameWindow.dockedState === GatesFrameState.DockedOnBottom)
                    ? Screen.virtualY + Screen.desktopAvailableHeight - namebar.height
                    : frameWindow.y
            }
        }
        ParallelAnimation {
            NumberAnimation {
                target: frameWindow; property: "height"
                duration: 200; easing.type: Easing.InOutQuad; to: namebar.height
            }
            NumberAnimation {
                id: collapseYAnim
                target: frameWindow; property: "y"
                duration: 200; easing.type: Easing.InOutQuad; to: 0
            }
        }
    }

    SequentialAnimation {
        id: maximiseAnimation
        ScriptAction {
            script: {
                expandYAnim.to = (frameWindow.dockedState === GatesFrameState.DockedOnBottom)
                    ? Screen.virtualY + Screen.desktopAvailableHeight - frameWindow.maximisedHeight
                    : frameWindow.y
            }
        }
        ParallelAnimation {
            NumberAnimation {
                target: frameWindow; property: "height"
                duration: 200; easing.type: Easing.InOutQuad; to: maximisedHeight
            }
            NumberAnimation {
                id: expandYAnim
                target: frameWindow; property: "y"
                duration: 200; easing.type: Easing.InOutQuad; to: 0
            }
        }
        PropertyAction { target: frameWindow; property: "minimumHeight"; value: namebar.height + 100 }
    }

    function minimise() {
        maximisedHeight = height
        minimised = true
        minimiseAnimation.start()
    }

    function maximise() {
        minimised = false
        maximiseAnimation.start()
    }

    // Instant collapse — used when restoring collapsed state from config (no animation on launch)
    function minimiseInstant() {
        maximisedHeight = height
        minimised       = true
        minimumHeight   = namebar.height
        height          = namebar.height
        if (dockedState === GatesFrameState.DockedOnBottom)
            y = Screen.virtualY + Screen.desktopAvailableHeight - namebar.height
    }

    // ── Dock-to-edge detection (runs after each window drag) ──────────────────
    readonly property int _dockThreshold: 20

    function checkAndDock() {
        const sTop    = Screen.virtualY
        const sBottom = Screen.virtualY + Screen.desktopAvailableHeight

        if (frameWindow.y <= sTop + _dockThreshold) {
            dockedState     = GatesFrameState.DockedOnTop
            nameBarPosition = GatesFrameState.NameBarOnBottom
            frameWindow.y   = sTop
            minimise()
        } else if (frameWindow.y + frameWindow.height >= sBottom - _dockThreshold) {
            dockedState     = GatesFrameState.DockedOnBottom
            nameBarPosition = GatesFrameState.NameBarOnTop
            minimise()
        }
    }

    // ── Auto-expand / collapse on hover when docked ───────────────────────────
    HoverHandler { id: windowHover }

    // Collapse after hover leaves, but not while the frame has OS focus
    // (i.e., the user has clicked something inside and it's still active).
    Timer {
        id: collapseTimer
        interval: 1000
        repeat:   false
        onTriggered: {
            if (dockedState === GatesFrameState.NotDocked || minimised) return
            if (frameWindow.active) {
                collapseTimer.restart()  // frame is focused; keep open and recheck later
            } else {
                minimise()
            }
        }
    }

    Connections {
        target: windowHover
        function onHoveredChanged() {
            if (dockedState === GatesFrameState.NotDocked) return
            if (windowHover.hovered) {
                collapseTimer.stop()
                if (minimised) maximise()
            } else {
                if (!minimised) collapseTimer.restart()
            }
        }
    }

    // ── Visuals ───────────────────────────────────────────────────────────────
    RoundedRect {
        id: iconViewBackground
        anchors.fill: parent
        antialiasing: true
        color:   backgroundSettings.color
        radius:  backgroundSettings.radius
        opacity: backgroundSettings.opacity
        roundedCorners: {
            switch (frameWindow.dockedState) {
            case GatesFrameState.DockedOnTop:    return RoundedRect.BottomCorners
            case GatesFrameState.DockedOnBottom: return RoundedRect.TopCorners
            default:                             return RoundedRect.AllCorners
            }
        }
    }

    RoundedRect {
        id: nameBarBackground
        anchors.fill: namebar
        radius:  backgroundSettings.radius
        color:   "black"
        opacity: Math.min(1.0, backgroundSettings.opacity)
        roundedCorners: {
            if (frameWindow.minimised)
                return RoundedRect.AllCorners
            switch (frameWindow.nameBarPosition) {
            case GatesFrameState.NameBarOnTop:    return RoundedRect.TopCorners
            case GatesFrameState.NameBarOnBottom: return RoundedRect.BottomCorners
            default:                              return RoundedRect.AllCorners
            }
        }
    }

    // Drop target highlight
    Rectangle {
        anchors.fill: parent
        color:   "white"
        radius:  backgroundSettings.radius
        opacity: dragDropService.active && dragDropService.hoveredFrameId === frameId &&
                 dragDropService.sourceFrameId !== frameId ? 0.12 : 0
        visible: opacity > 0
        z: 1
        Behavior on opacity { NumberAnimation { duration: 120 } }
    }

    // ── Icon grid ─────────────────────────────────────────────────────────────
    // Use y/height instead of anchors to avoid a one-frame glitch where both
    // top and bottom anchors are briefly set simultaneously during transitions.
    Item {
        id: iconView
        width:  parent.width
        y:      nameBarPosition === GatesFrameState.NameBarOnTop ? namebar.height : 0
        height: parent.height - namebar.height

        IconGridView {
            id: grid
            anchors.fill: parent
            model: my_fileModel
        }
    }

    // ── Name bar ──────────────────────────────────────────────────────────────
    FrameNameBar {
        id: namebar
        width:  parent.width
        height: 40
        anchors.top: parent.top  // default: namebar on top
        anchors.horizontalCenter: parent.horizontalCenter

        states: State {
            name: "onBottom"
            when: nameBarPosition === GatesFrameState.NameBarOnBottom
            AnchorChanges {
                target: namebar
                anchors.top:    undefined
                anchors.bottom: namebar.parent.bottom
            }
        }

        text:            frameWindow.title
        frameMinimised:  frameWindow.minimised
        nameBarPosition: frameWindow.nameBarPosition

        onMinimiseButtonTriggered: {
            if (minimised) maximise()
            else           minimise()
        }

        onOptionsButtonTriggered: {
            // TODO: open options panel
        }

        DragHandler {
            target: frameWindow
            grabPermissions: PointerHandler.CanTakeOverFromItems |
                             PointerHandler.CanTakeOverFromHandlersOfDifferentType |
                             PointerHandler.ApprovesTakeOverByAnything
            dragThreshold: 0
            onActiveChanged: {
                if (active) {
                    // Undock and restore to full size before allowing drag
                    if (dockedState !== GatesFrameState.NotDocked) {
                        collapseTimer.stop()
                        if (dockedState === GatesFrameState.DockedOnBottom)
                            frameWindow.y = Screen.virtualY + Screen.desktopAvailableHeight - maximisedHeight
                        dockedState     = GatesFrameState.NotDocked
                        nameBarPosition = GatesFrameState.NameBarOnTop
                        minimised       = false
                        minimumHeight   = namebar.height + 100
                        height          = maximisedHeight
                    }
                    startSystemMove()
                } else {
                    checkAndDock()
                }
            }
        }
    }
}
