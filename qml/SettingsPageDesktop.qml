import QtQuick
import QtQuick.Controls

Item {
    id: page

    Column {
        anchors { top: parent.top; left: parent.left; right: parent.right; margins: 28 }
        spacing: 0

        // ── Header ────────────────────────────────────────────────────────────
        Text {
            text: "Рабочий стол"
            color: "white"; font.family: "Tex Gyre Adventor"
            font.pixelSize: 22; font.bold: true
        }
        Item { width: 1; height: 4 }
        Text {
            text: "Настройки сетки иконок · изменения применяются мгновенно"
            color: Qt.rgba(1,1,1,0.45); font.family: "Tex Gyre Adventor"; font.pixelSize: 13
        }
        Item { width: 1; height: 24 }

        // ── Section: frames ───────────────────────────────────────────────────
        Text {
            text: "ФРЕЙМЫ"
            color: Qt.rgba(1,1,1,0.45); font.family: "Tex Gyre Adventor"
            font.pixelSize: 10; font.bold: true; font.letterSpacing: 1.5
        }
        Item { width: 1; height: 12 }

        // Snap frames toggle
        Row {
            height: 36; spacing: 0
            Text {
                text: "Привязка фреймов к сетке"
                color: Qt.rgba(1,1,1,0.75); font.family: "Tex Gyre Adventor"; font.pixelSize: 13
                width: 180; height: parent.height; verticalAlignment: Text.AlignVCenter
            }
            Rectangle {
                id: snapToggle
                width: 44; height: 24; radius: 12
                anchors.verticalCenter: parent.verticalCenter
                color: settingsBackend.snapFramesToGrid ? "#2575bf" : Qt.rgba(1,1,1,0.15)
                Behavior on color { ColorAnimation { duration: 150 } }
                Rectangle {
                    x: settingsBackend.snapFramesToGrid ? parent.width - width - 3 : 3
                    anchors.verticalCenter: parent.verticalCenter
                    width: 18; height: 18; radius: 9; color: "white"
                    Behavior on x { NumberAnimation { duration: 150; easing.type: Easing.InOutQuad } }
                }
                TapHandler { onTapped: settingsBackend.snapFramesToGrid = !settingsBackend.snapFramesToGrid }
            }
        }
        Item { width: 1; height: 24 }

        // ── Section: debug grid ───────────────────────────────────────────────
        Text {
            text: "ОТЛАДКА"
            color: Qt.rgba(1,1,1,0.45); font.family: "Tex Gyre Adventor"
            font.pixelSize: 10; font.bold: true; font.letterSpacing: 1.5
        }
        Item { width: 1; height: 12 }

        // Toggle row
        Row {
            height: 36; spacing: 0
            Text {
                text: "Показывать сетку"
                color: Qt.rgba(1,1,1,0.75); font.family: "Tex Gyre Adventor"; font.pixelSize: 13
                width: 180; height: parent.height; verticalAlignment: Text.AlignVCenter
            }
            // Toggle switch
            Rectangle {
                id: debugToggle
                width: 44; height: 24; radius: 12
                anchors.verticalCenter: parent.verticalCenter
                color: settingsBackend.showDebugGrid ? "#2575bf" : Qt.rgba(1,1,1,0.15)
                Behavior on color { ColorAnimation { duration: 150 } }

                Rectangle {
                    x: settingsBackend.showDebugGrid ? parent.width - width - 3 : 3
                    anchors.verticalCenter: parent.verticalCenter
                    width: 18; height: 18; radius: 9
                    color: "white"
                    Behavior on x { NumberAnimation { duration: 150; easing.type: Easing.InOutQuad } }
                }

                TapHandler {
                    onTapped: settingsBackend.showDebugGrid = !settingsBackend.showDebugGrid
                }
            }
        }
        Item { width: 1; height: 24 }

        // ── Section: grid count ───────────────────────────────────────────────
        Text {
            text: "КОЛИЧЕСТВО ЯЧЕЕК"
            color: Qt.rgba(1,1,1,0.45); font.family: "Tex Gyre Adventor"
            font.pixelSize: 10; font.bold: true; font.letterSpacing: 1.5
        }
        Item { width: 1; height: 12 }

        Row {
            height: 32; spacing: 8
            Text {
                text: "Колонок"
                color: Qt.rgba(1,1,1,0.75); font.family: "Tex Gyre Adventor"; font.pixelSize: 13
                width: 180; height: parent.height; verticalAlignment: Text.AlignVCenter
            }
            SettingsSpinBox {
                anchors.verticalCenter: parent.verticalCenter
                value: settingsBackend.desktopNumCols
                from: 3; to: 40
                onCommitted: (v) => { settingsBackend.desktopNumCols = v }
            }
            Text {
                text: "  →  " + settingsBackend.desktopCellW + " px"
                color: Qt.rgba(1,1,1,0.28); font.family: "Tex Gyre Adventor"; font.pixelSize: 11
                height: parent.height; verticalAlignment: Text.AlignVCenter
            }
        }
        Item { width: 1; height: 8 }
        Row {
            height: 32; spacing: 8
            Text {
                text: "Строк"
                color: Qt.rgba(1,1,1,0.75); font.family: "Tex Gyre Adventor"; font.pixelSize: 13
                width: 180; height: parent.height; verticalAlignment: Text.AlignVCenter
            }
            SettingsSpinBox {
                anchors.verticalCenter: parent.verticalCenter
                value: settingsBackend.desktopNumRows
                from: 2; to: 25
                onCommitted: (v) => { settingsBackend.desktopNumRows = v }
            }
            Text {
                text: "  →  " + settingsBackend.desktopCellH + " px"
                color: Qt.rgba(1,1,1,0.28); font.family: "Tex Gyre Adventor"; font.pixelSize: 11
                height: parent.height; verticalAlignment: Text.AlignVCenter
            }
        }
        Item { width: 1; height: 24 }

        // ── Section: gaps & margin ────────────────────────────────────────────
        Text {
            text: "ОТСТУПЫ"
            color: Qt.rgba(1,1,1,0.45); font.family: "Tex Gyre Adventor"
            font.pixelSize: 10; font.bold: true; font.letterSpacing: 1.5
        }
        Item { width: 1; height: 12 }

        Row {
            height: 32; spacing: 8
            Text { text: "Зазор по горизонтали"; color: Qt.rgba(1,1,1,0.75); font.family: "Tex Gyre Adventor"; font.pixelSize: 13; width: 180; height: parent.height; verticalAlignment: Text.AlignVCenter }
            SettingsSpinBox { anchors.verticalCenter: parent.verticalCenter; value: settingsBackend.desktopGapX; from: 0; to: 80; onCommitted: (v) => { settingsBackend.desktopGapX = v } }
            Text { text: "px"; color: Qt.rgba(1,1,1,0.30); font.family: "Tex Gyre Adventor"; font.pixelSize: 12; height: parent.height; verticalAlignment: Text.AlignVCenter }
        }
        Item { width: 1; height: 8 }
        Row {
            height: 32; spacing: 8
            Text { text: "Зазор по вертикали"; color: Qt.rgba(1,1,1,0.75); font.family: "Tex Gyre Adventor"; font.pixelSize: 13; width: 180; height: parent.height; verticalAlignment: Text.AlignVCenter }
            SettingsSpinBox { anchors.verticalCenter: parent.verticalCenter; value: settingsBackend.desktopGapY; from: 0; to: 80; onCommitted: (v) => { settingsBackend.desktopGapY = v } }
            Text { text: "px"; color: Qt.rgba(1,1,1,0.30); font.family: "Tex Gyre Adventor"; font.pixelSize: 12; height: parent.height; verticalAlignment: Text.AlignVCenter }
        }
        Item { width: 1; height: 8 }
        Row {
            height: 32; spacing: 8
            Text { text: "Поля от краёв экрана"; color: Qt.rgba(1,1,1,0.75); font.family: "Tex Gyre Adventor"; font.pixelSize: 13; width: 180; height: parent.height; verticalAlignment: Text.AlignVCenter }
            SettingsSpinBox { anchors.verticalCenter: parent.verticalCenter; value: settingsBackend.desktopMargin; from: 0; to: 100; onCommitted: (v) => { settingsBackend.desktopMargin = v } }
            Text { text: "px"; color: Qt.rgba(1,1,1,0.30); font.family: "Tex Gyre Adventor"; font.pixelSize: 12; height: parent.height; verticalAlignment: Text.AlignVCenter }
        }
    }
}
