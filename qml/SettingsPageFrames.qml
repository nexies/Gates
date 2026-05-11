import QtQuick
import QtQuick.Controls

Item {
    id: page

    // ── Header ────────────────────────────────────────────────────────────────
    Column {
        id: header
        anchors { top: parent.top; left: parent.left; right: parent.right }
        anchors.margins: 28
        spacing: 4

        Text {
            text: "Фреймы"
            color: "white"; font.family: "Tex Gyre Adventor"
            font.pixelSize: 22; font.bold: true
        }
        Text {
            text: "Управление папками на рабочем столе"
            color: Qt.rgba(1,1,1,0.45); font.family: "Tex Gyre Adventor"; font.pixelSize: 13
        }
    }

    // ── Separator ─────────────────────────────────────────────────────────────
    Rectangle {
        id: headerSep
        anchors { top: header.bottom; topMargin: 16; left: parent.left; right: parent.right; leftMargin: 28; rightMargin: 28 }
        height: 1; color: Qt.rgba(1,1,1,0.08)
    }

    // ── Frames list ───────────────────────────────────────────────────────────
    ListView {
        id: framesList
        anchors {
            top: headerSep.bottom; topMargin: 12
            left: parent.left; right: parent.right; bottom: addBtn.top
            leftMargin: 28; rightMargin: 28; bottomMargin: 12
        }
        model: settingsBackend.frames
        clip: true
        spacing: 8

        delegate: Rectangle {
            id: card

            property string frameId:  modelData.id
            property string frameName: modelData.name
            property string frameDir:  modelData.dir

            width: framesList.width
            height: 64
            radius: 8
            color: cardHov.containsMouse ? Qt.rgba(1,1,1,0.09) : Qt.rgba(1,1,1,0.06)
            Behavior on color { ColorAnimation { duration: 120 } }

            // ── Frame name (editable) ─────────────────────────────────────
            TextInput {
                id: nameInput
                anchors { top: parent.top; left: parent.left; right: deleteBtn.left
                          topMargin: 12; leftMargin: 14; rightMargin: 8 }
                text: card.frameName
                color: "white"
                selectionColor: "#2575bf"
                font.family: "Tex Gyre Adventor"; font.pixelSize: 14; font.bold: true
                selectByMouse: true
                clip: true
                onEditingFinished: {
                    if (text.trim().length > 0 && text !== card.frameName)
                        settingsBackend.setFrameName(card.frameId, text.trim())
                    else
                        text = card.frameName
                }
            }

            // ── Directory path + change button ───────────────────────────
            Row {
                anchors { bottom: parent.bottom; left: parent.left; right: deleteBtn.left
                          bottomMargin: 10; leftMargin: 14; rightMargin: 8 }
                spacing: 8

                Text {
                    id: dirText
                    text: card.frameDir.length > 0 ? card.frameDir : "Папка не задана"
                    color: card.frameDir.length > 0 ? Qt.rgba(1,1,1,0.5) : Qt.rgba(1,0.4,0.4,0.7)
                    font.family: "Tex Gyre Adventor"; font.pixelSize: 12
                    elide: Text.ElideMiddle
                    width: Math.min(implicitWidth, card.width - deleteBtn.width - changeDirBtn.width - 60)
                }

                Rectangle {
                    id: changeDirBtn
                    width: changeDirRow.implicitWidth + 16; height: 22; radius: 4
                    color: dirHov.containsMouse ? Qt.rgba(1,1,1,0.18) : Qt.rgba(1,1,1,0.10)
                    Behavior on color { ColorAnimation { duration: 100 } }
                    anchors.verticalCenter: dirText.verticalCenter

                    Row {
                        id: changeDirRow
                        anchors.centerIn: parent
                        spacing: 5

                        Image {
                            source: "qrc:/icons/open_with.png"
                            width: 12; height: 12
                            anchors.verticalCenter: parent.verticalCenter
                            opacity: 0.7
                        }
                        Text {
                            text: "Изменить"
                            color: "white"; font.family: "Tex Gyre Adventor"; font.pixelSize: 11
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    HoverHandler { id: dirHov }
                    TapHandler {
                        onTapped: {
                            const dir = settingsBackend.pickDirectory()
                            if (dir.length > 0)
                                settingsBackend.setFrameDir(card.frameId, dir)
                        }
                    }
                }
            }

            // ── Delete button ─────────────────────────────────────────────
            Rectangle {
                id: deleteBtn
                anchors { right: parent.right; rightMargin: 10; verticalCenter: parent.verticalCenter }
                width: 28; height: 28; radius: 6
                color: delHov.containsMouse ? Qt.rgba(1,0.2,0.2,0.40) : "transparent"
                Behavior on color { ColorAnimation { duration: 120 } }

                Image {
                    anchors.centerIn: parent
                    source: "qrc:/icons/close.png"
                    width: 13; height: 13
                    opacity: delHov.containsMouse ? 1.0 : 0.35
                    Behavior on opacity { NumberAnimation { duration: 120 } }
                }

                HoverHandler { id: delHov }
                TapHandler { onTapped: settingsBackend.removeFrame(card.frameId) }
            }

            HoverHandler { id: cardHov }
        }

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
            contentItem: Rectangle {
                radius: 3; color: Qt.rgba(1,1,1,0.25)
            }
        }
    }

    // ── Add frame button ──────────────────────────────────────────────────────
    Rectangle {
        id: addBtn
        anchors { bottom: parent.bottom; left: parent.left; right: parent.right
                  bottomMargin: 24; leftMargin: 28; rightMargin: 28 }
        height: 40; radius: 8
        color: addHov.containsMouse ? "#2177cc" : "#1a6fc4"
        Behavior on color { ColorAnimation { duration: 120 } }

        Row {
            anchors.centerIn: parent
            spacing: 8

            Image {
                source: "qrc:/icons/plus_filled.png"
                width: 16; height: 16
                anchors.verticalCenter: parent.verticalCenter
                opacity: 0.9
            }
            Text {
                text: "Добавить фрейм"
                color: "white"; font.family: "Tex Gyre Adventor"
                font.pixelSize: 14; font.bold: true
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        HoverHandler { id: addHov }
        TapHandler {
            onTapped: {
                const dir = settingsBackend.pickDirectory()
                if (dir.length > 0)
                    settingsBackend.addFrame("", dir)
            }
        }
    }
}
