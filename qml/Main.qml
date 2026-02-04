import QtQuick
import QtQuick.Controls
import CustomQmlTypes
import QtQml.Models


Window {
    // property string name: "GatesFrame"
    // objectName: "GatesFrame"

    id: viewPort
    width: 600
    height: 300
    visible: true
    title: qsTr("Hello, Frame!")

    flags: Qt.Widget | Qt.FramelessWindowHint
    color: "transparent"

    property alias frameOpacity: background.opacity
    property alias frameColor: background.color
    property alias frameRadius: background.radius

    RoundedRect
    {
        id: background

        antialiasing: true
        anchors.fill: parent
        color: "black"
        radius: 5
        opacity: 0.7
        roundedCorners: RoundedRect.AllCorners
    }

    FrameNameBar
    {
        state: "VisibleTop"
        radius: frameRadius
        DragHandler
        {
            target: viewPort
            onActiveChanged: if(active) startSystemMove();
        }
    }


}

/*
Window {

    id: frame
    width: 600
    height: 300
    visible: true
    title: qsTr("Hello World")

    color: "transparent"
    flags: Qt.Widget | Qt.FramelessWindowHint

    RoundedRect
    {
        id: background
        antialiasing: true
        anchors.fill: parent
        color: "black"
        radius: 5
        opacity: 0.6
        roundedCorners: RoundedRect.AllCorners
    }

    FrameNameBar
    {
        opacity: 1
        // anchors.bottom: parent.bottom
        state: "VisibleTop"
        width: parent.width
        radius: background.radius
        text: frame.title
    }

    DragHandler
    {
        onActiveChanged: if(active) startSystemMove();
    }


/*
    x: 0
    y: 0

    property int dockedState: 0

    onYChanged:
    {
        if(y < 15)
        {
            console.log("top border!");
            dockedState = 1;
            y = 0;
        }
    }

    Rectangle
    {
        anchors.fill: parent
        id: backgroundFill
        color: "black"
        opacity: 0.5
        radius: 10
        border
        {
            width: 10
            color: "black"
        }
    }


    ListModel
    {
        id: contactModel
        ListElement {
            name: "Jim Williams"
            colorName: "blue"
        }
        ListElement {
            name: "John Brown"
            colorName: "red"
        }
        ListElement {
            name: "Bill Smyth"
            colorName: "green"
        }
        ListElement {
            name: "Sam Wise"
            colorName: "magenta"
        }
        ListElement {
            name: "Jim Williams"
            colorName: "blue"
        }
        ListElement {
            name: "John Brown"
            colorName: "red"
        }
        ListElement {
            name: "Bill Smyth"
            colorName: "green"
        }
        ListElement {
            name: "Sam Wise"
            colorName: "magenta"
        }
        ListElement {
            name: "Jim Williams"
            colorName: "blue"
        }
        ListElement {
            name: "John Brown"
            colorName: "red"
        }
        ListElement {
            name: "Bill Smyth"
            colorName: "green"
        }
        ListElement {
            name: "Sam Wise"
            colorName: "magenta"
        }
        ListElement {
            name: "Jim Williams"
            colorName: "blue"
        }
        ListElement {
            name: "John Brown"
            colorName: "red"
        }
        ListElement {
            name: "Bill Smyth"
            colorName: "green"
        }
        ListElement {
            name: "Sam Wise"
            colorName: "magenta"
        }
        ListElement {
            name: "Jim Williams"
            colorName: "blue"
        }
        ListElement {
            name: "John Brown"
            colorName: "red"
        }
        ListElement {
            name: "Bill Smyth"
            colorName: "green"
        }
        ListElement {
            name: "Sam Wise"
            colorName: "magenta"
        }

    }


    Rectangle {
        anchors.fill: parent
        color: "transparent"

        Component {
            id: contactDelegate
            Item {
                width: grid.cellWidth; height: grid.cellHeight
                Column {
                    anchors.fill: parent
                    Rectangle { color: colorName; height: 50; width: 50; anchors.horizontalCenter: parent.horizontalCenter }
                    Text { text: name; color: "white"; anchors.horizontalCenter: parent.horizontalCenter }
                }
            }
        }

        GridView {
            id: grid
            anchors.fill: parent
            cellWidth: 80; cellHeight: 80

            model: contactModel
            delegate: contactDelegate
            highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
            focus: true
        }
    }
}
    */

