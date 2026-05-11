import QtQuick
import QtQuick.Controls
import QtQuick.Window
import Gates

Window {
    id: root

    width:       800
    height:      540
    minimumWidth:  800
    minimumHeight: 540
    flags: Qt.Window | Qt.FramelessWindowHint
    color: "transparent"
    title: "Gates — Настройки"

    onClosing: (close) => {
        close.accepted = false
        visible = false
    }

    property int currentPage: 0

    // ── Outer border / glow ───────────────────────────────────────────────────
    Rectangle {
        anchors.fill: parent
        color: "transparent"; radius: 12
        border.color: Qt.rgba(1,1,1,0.09); border.width: 1
        z: 10
    }

    // ── Main container ────────────────────────────────────────────────────────
    Rectangle {
        anchors.fill: parent
        color: "#0f0f1e"; radius: 12; clip: true

        // ── Title bar ─────────────────────────────────────────────────────────
        Rectangle {
            id: titleBar
            anchors { top: parent.top; left: parent.left; right: parent.right }
            height: 44
            color: "#080814"

            DragHandler { onActiveChanged: if (active) root.startSystemMove() }

            Row {
                anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: 16 }
                spacing: 8

                Image {
                    source: "qrc:/icons/settings_filled.png"
                    width: 15; height: 15
                    anchors.verticalCenter: parent.verticalCenter
                    opacity: 0.55
                }
                Text {
                    text: "Gates — Настройки"
                    color: "white"; font.family: "Tex Gyre Adventor"
                    font.pixelSize: 14; font.bold: true; opacity: 0.85
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Rectangle {
                anchors { right: parent.right; rightMargin: 8; verticalCenter: parent.verticalCenter }
                width: 28; height: 28; radius: 6
                color: closeHov.containsMouse ? Qt.rgba(1,0.15,0.15,0.45) : "transparent"
                Behavior on color { ColorAnimation { duration: 120 } }

                Image {
                    anchors.centerIn: parent
                    source: "qrc:/icons/close.png"
                    width: 13; height: 13
                    opacity: closeHov.containsMouse ? 1.0 : 0.40
                    Behavior on opacity { NumberAnimation { duration: 120 } }
                }

                HoverHandler { id: closeHov }
                TapHandler    { onTapped: root.visible = false }
            }

            Rectangle {
                anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
                height: 1; color: Qt.rgba(1,1,1,0.07)
            }
        }

        // ── Body row: sidebar + content ───────────────────────────────────────
        Row {
            anchors {
                top: titleBar.bottom; left: parent.left
                right: parent.right; bottom: parent.bottom
            }

            // ── Sidebar ───────────────────────────────────────────────────────
            Rectangle {
                width: 200; height: parent.height
                color: "#080814"

                Rectangle {
                    anchors { right: parent.right; top: parent.top; bottom: parent.bottom }
                    width: 1; color: Qt.rgba(1,1,1,0.07)
                }

                Column {
                    anchors { top: parent.top; topMargin: 10; left: parent.left; right: parent.right; rightMargin: 1 }
                    spacing: 2

                    Repeater {
                        model: ListModel {
                            ListElement { navIcon: "menu.png";  navLabel: "Фреймы";        navPage: 0 }
                            ListElement { navIcon: "home.png";  navLabel: "Рабочий стол";  navPage: 1 }
                            ListElement { navIcon: "star.png";  navLabel: "Внешний вид";   navPage: 2 }
                            ListElement { navIcon: "bolt.png";  navLabel: "О приложении";  navPage: 3 }
                        }

                        delegate: Rectangle {
                            width: parent.width; height: 42
                            color: root.currentPage === navPage
                                   ? Qt.rgba(1,1,1,0.12)
                                   : (navHov.containsMouse ? Qt.rgba(1,1,1,0.06) : "transparent")
                            Behavior on color { ColorAnimation { duration: 120 } }

                            Rectangle {
                                anchors { left: parent.left; top: parent.top; bottom: parent.bottom }
                                width: 3; radius: 2; color: "#2575bf"
                                visible: root.currentPage === navPage
                            }

                            Row {
                                anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: 20 }
                                spacing: 10

                                Image {
                                    source: "qrc:/icons/" + navIcon
                                    width: 15; height: 15
                                    anchors.verticalCenter: parent.verticalCenter
                                    opacity: root.currentPage === navPage ? 0.90 : 0.40
                                    Behavior on opacity { NumberAnimation { duration: 120 } }
                                }
                                Text {
                                    text: navLabel
                                    color: "white"; font.family: "Tex Gyre Adventor"; font.pixelSize: 13
                                    font.bold: root.currentPage === navPage
                                    opacity: root.currentPage === navPage ? 1.0 : 0.55
                                    anchors.verticalCenter: parent.verticalCenter
                                    Behavior on opacity { NumberAnimation { duration: 120 } }
                                }
                            }

                            HoverHandler { id: navHov }
                            TapHandler   { onTapped: root.currentPage = navPage }
                        }
                    }
                }
            }

            // ── Content area ──────────────────────────────────────────────────
            Item {
                width: parent.width - 200; height: parent.height

                SettingsPageFrames  { anchors.fill: parent; visible: root.currentPage === 0 }
                SettingsPageDesktop { anchors.fill: parent; visible: root.currentPage === 1 }

                // Appearance placeholder
                Item {
                    anchors.fill: parent; visible: root.currentPage === 2
                    Text {
                        anchors.centerIn: parent
                        text: "Скоро..."
                        color: Qt.rgba(1,1,1,0.25); font.family: "Tex Gyre Adventor"; font.pixelSize: 18
                    }
                }

                SettingsPageAbout { anchors.fill: parent; visible: root.currentPage === 3 }
            }
        }
    }
}
