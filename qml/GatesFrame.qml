import QtQuick
import QtQuick.Controls
import CustomQmlTypes

ResizableFramelessWindow {
    property string name: "GatesFrame"
    id: frameWindow
    title: qsTr("Gates Frame")
    visible: true
    color: "transparent"

    width: 600
    height: 500
    minimumHeight: namebar.height + 100
    minimumWidth: 100
    property int maximisedHeight: 500

    blurBehind: true

    resizeSnapToGrid: true
    sizeIncrement: Qt.size(100, 100)
    baseSize: Qt.size(100, 100)

    property int nameBarPosition: GatesFrameState.NameBarOnBottom
    property int dockedState: GatesFrameState.NotDocked
    property alias frameColor: backgroundSettings.color
    property bool minimised: false

    SequentialAnimation
    {
        id: minimiseAnimation

        PropertyAction { target: frameWindow; property: "minimumHeight"; value: namebar.height }

        NumberAnimation {
            target: frameWindow
            property: "height"
            duration: 200
            easing.type: Easing.InOutQuad
            to: namebar.height - 1
        }
    }

    SequentialAnimation
    {
        id: maximiseAnimation

        NumberAnimation {
            target: frameWindow
            property: "height"
            duration: 200
            easing.type: Easing.InOutQuad
            to: maximisedHeight
        }

        PropertyAction { target: frameWindow; property: "minimumHeight"; value: namebar.height + 100 }
    }

    function minimise(min = true)
    {
        if(!min)
            maximise(min)

        maximisedHeight = height;
        minimised = true;
        minimiseAnimation.start()
    }

    function maximise(max = true)
    {
        if(!max)
            minimise(max)
        minimised = false;
        maximiseAnimation.start()
    }


    QtObject
    {
        id: backgroundSettings
        property color color: "blue"
        property int radius: 0
        property double opacity: 0.2
    }

    RoundedRect
    {
        id: iconViewBackground
        antialiasing: true

        anchors.fill: parent
        color: "blue"
        radius: backgroundSettings.radius
        opacity: backgroundSettings.opacity
        roundedCorners: {
            if(frameWindow.dockedState == GatesFrameState.NotDocked)
                return RoundedRect.AllCorners
            else if(frameWindow.dockedState == GatesFrameState.DockedOnBottom)
                return RoundedRect.TopCorners
            else if(frameWindow.dockedState == GatesFrameState.DockedOnTop)
                return RoundedRect.BottomCorners
            else
                return RoundedRect.NoCorners
        }
    }
    RoundedRect
    {
        id: nameBarBackground
        radius: backgroundSettings.radius

        anchors.fill: namebar
        color: "black"
        opacity: backgroundSettings.opacity + 0.3
        roundedCorners: {
            if(frameWindow.minimised)
                return RoundedRect.AllCorners

            if(frameWindow.nameBarPosition === GatesFrameState.NameBarOnTop)
                return RoundedRect.TopCorners
            if(frameWindow.nameBarPosition === GatesFrameState.NameBarOnBottom)
                return RoundedRect.BottomCorners
            else
                return RoundedRect.AllCorners
        }
    }

    Item
    {
        id: iconView
        width: parent.width
        visible: true
        // anchors.top: {
        //     if (frameWindow.nameBarPosition == GatesFrameState.NameBarOnTop) {
        //         console.log("Icon view top = namebar.bottom")
        //         return namebar.bottom;
        //     }
        //     if (frameWindow.nameBarPosition == GatesFrameState.NameBarOnBottom) {
        //         console.log("Icon view top = parent.top")
        //         return parent.top;
        //     }
        //     console.log("Icon view top = unknown")
        // }

        // anchors.bottom: {
        //     if (frameWindow.nameBarPosition == GatesFrameState.NameBarOnTop)
        //         return parent.bottom;
        //     if (frameWindow.nameBarPosition == GatesFrameState.NameBarOnBottom)
        //         return namebar.top;
        // }

        anchors.top: namebar.bottom
        anchors.bottom: parent.bottom


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
        x: 0
        y: 0
        text: frameWindow.title
        id: namebar
        anchors.top: frameWindow.top
        anchors.horizontalCenter: frameWindow.horizontalCenter
        frameMinimised: frameWindow.minimised

        onMinimiseButtonTriggered: {
            if(frameWindow.minimised)
                maximise()
            else
                minimise()
            nameBarBackground.update()

        }

        onOptionsButtonTriggered: {
            debugRect.visible = !(debugRect.visible)
        }

        DragHandler {
            target: frameWindow
            grabPermissions:  PointerHandler.CanTakeOverFromItems | PointerHandler.CanTakeOverFromHandlersOfDifferentType | PointerHandler.ApprovesTakeOverByAnything
            dragThreshold: 0
            onActiveChanged: {
                if(active) startSystemMove();
                else
                {
                    namebar.x = 0;
                    namebar.y = 0;
                }
            }
        }

    }


}
