import QtQuick
import QtQuick.Controls
import CustomQmlTypes


ResizableFramelessWindow
{
    id: frameWindow
    width: 600
    minimumHeight: 50
    minimumWidth: 50
    blurBehind: true

    sizeIncrement: Qt.size(100, 100)
    baseSize: Qt.size(grid.anchors.leftMargin + grid.anchors.rightMargin + grid.cellWidth,
                      grid.anchors.leftMargin + grid.anchors.rightMargin + grid.cellWidth + namebar.height)


    visible: true
    title: qsTr("Frame")
    color: "transparent"

    height:
    {
        if(frameMinimised)
            return namebar.height
        else
            return 300
    }

    Behavior on height { NumberAnimation { easing.type: Easing.InOutQuad; duration: 200 }}


    property alias frameRadius: viewPort.frameRadius
    property alias frameOpacity: viewPort.frameOpacity
    property alias frameColor: viewPort.frameColor
    property alias nameBarPosition: viewPort.nameBarPosition
    property alias doeckedState: viewPort.dockedState
    property alias frameMinimised: viewPort.minimised



    Item
    {
        id: viewPort
        property alias frameRadius: background.radius
        frameRadius: 8

        property alias frameOpacity: background.opacity
        property alias frameColor: background.color
        property int nameBarPosition: GatesFrameState.NameBarOnTop
        property int dockedState: GatesFrameState.NotDocked

        property bool minimised: false

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top


        height: {
            if(minimised)
                return namebar.height
            else
                return parent.height
        }

        Behavior on height { NumberAnimation { easing.type: Easing.InOutQuad; duration: 200 }}

        Item {
            id: background
            property int radius: 5
            property color color: "black"
        }

        RoundedRect
        {
            id: iconViewBackground
            antialiasing: true

            anchors.fill: viewPort
            color: background.color
            radius: background.radius
            opacity: background.opacity
            roundedCorners: {
                if(viewPort.dockedState == GatesFrameState.NotDocked)
                    return RoundedRect.AllCorners
                else if(viewPort.dockedState == GatesFrameState.DockedOnBottom)
                    return RoundedRect.TopCorners
                else if(viewPort.dockedState == GatesFrameState.DockedOnTop)
                    return RoundedRect.BottomCorners
                else
                    return RoundedRect.NoCorners
            }
        }

        RoundedRect
        {
            id: nameBarBackground
            radius: background.radius

            anchors.fill: namebar
            color: "black"
            opacity: background.opacity
            roundedCorners: {
                if(viewPort.minimised)
                    return RoundedRect.AllCorners

                if(viewPort.nameBarPosition === GatesFrameState.NameBarOnTop)
                    return RoundedRect.TopCorners
                if(viewPort.nameBarPosition === GatesFrameState.NameBarOnBottom)
                    return RoundedRect.BottomCorners
                else
                    return RoundedRect.AllCorners

            }
        }

        Item
        {
            id: iconView
            width: parent.width
            anchors.top: {
                if (viewPort.nameBarPosition == GatesFrameState.NameBarOnTop) {
                    console.log("Icon view top = namebar.bottom")
                    return namebar.bottom;
                }
                if (viewPort.nameBarPosition == GatesFrameState.NameBarOnBottom) {
                    console.log("Icon view top = parent.top")
                    return parent.top;
                }
                console.log("Icon view top = unknown")
            }

            anchors.bottom: {
                if (viewPort.nameBarPosition == GatesFrameState.NameBarOnTop)
                    return parent.bottom;
                if (viewPort.nameBarPosition == GatesFrameState.NameBarOnBottom)
                    return namebar.top;
            }

            layer.enabled: true

            IconGridView
            {
                id: grid
                anchors.fill: iconView
                width: iconView.width
                model: my_fileModel
            }

        }

        FrameNameBar
        {
            // barPosition: viewPort.nameBarPosition
            // radius: viewPort.frameRadius
            text: frameWindow.title
            id: namebar
            anchors.top: frameWindow.top

            onMinimiseButtonTriggered:
            {
                viewPort.minimised = !(viewPort.minimised)
            }

            DragHandler {
                // acceptedDevices: PointerDevice.GenericPointer
                grabPermissions:  PointerHandler.CanTakeOverFromItems | PointerHandler.CanTakeOverFromHandlersOfDifferentType | PointerHandler.ApprovesTakeOverByAnything
                onActiveChanged: if (active) frameWindow.startSystemMove()
            }
        }
    }
}
