import QtQuick

// Styled integer spin box matching the Gates dark theme.
Row {
    id: control

    property int value: 0
    property int from:  1
    property int to:    9999

    signal committed(int v)

    height: 30
    spacing: 2

    // ── Decrease ──────────────────────────────────────────────────────────────
    Rectangle {
        width: 26; height: parent.height; radius: 5
        color: dh.containsMouse ? Qt.rgba(1,1,1,0.20) : Qt.rgba(1,1,1,0.10)
        Behavior on color { ColorAnimation { duration: 100 } }

        Text {
            anchors.centerIn: parent
            text: "−"; color: "white"; font.pixelSize: 16; font.bold: true
        }

        HoverHandler { id: dh }
        TapHandler {
            onTapped: {
                const v = Math.max(control.from, control.value - 1)
                control.value = v
                control.committed(v)
            }
        }
    }

    // ── Input field ───────────────────────────────────────────────────────────
    Rectangle {
        width: 60; height: parent.height
        color: Qt.rgba(1,1,1,0.07); radius: 4

        TextInput {
            id: input
            anchors { fill: parent; leftMargin: 4; rightMargin: 4 }
            verticalAlignment:   Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            color: "white"
            selectionColor: "#2575bf"
            font.family: "Tex Gyre Adventor"; font.pixelSize: 13
            selectByMouse: true
            validator: IntValidator { bottom: control.from; top: control.to }
            onEditingFinished: {
                const parsed = parseInt(text)
                const v = isNaN(parsed)
                          ? control.from
                          : Math.max(control.from, Math.min(control.to, parsed))
                control.value = v
                control.committed(v)
                text = String(v)
            }
        }

        // Keep text in sync with value unless the user is actively editing.
        Binding {
            target: input; property: "text"; value: String(control.value)
            when: !input.activeFocus
        }
    }

    // ── Increase ──────────────────────────────────────────────────────────────
    Rectangle {
        width: 26; height: parent.height; radius: 5
        color: uh.containsMouse ? Qt.rgba(1,1,1,0.20) : Qt.rgba(1,1,1,0.10)
        Behavior on color { ColorAnimation { duration: 100 } }

        Text {
            anchors.centerIn: parent
            text: "+"; color: "white"; font.pixelSize: 16; font.bold: true
        }

        HoverHandler { id: uh }
        TapHandler {
            onTapped: {
                const v = Math.min(control.to, control.value + 1)
                control.value = v
                control.committed(v)
            }
        }
    }
}
