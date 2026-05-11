#pragma once
#include <QObject>

namespace Gates {

// Live bridge between DesktopLayoutConfig and QML.
// Exposed as context property "desktopConfig" on the root QML context so that
// VirtualDesktop.qml and any other QML can bind to layout values and react to changes.
class DesktopConfigBridge : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int  cellW         READ cellW         NOTIFY layoutChanged)
    Q_PROPERTY(int  cellH         READ cellH         NOTIFY layoutChanged)
    Q_PROPERTY(int  gapX          READ gapX          NOTIFY layoutChanged)
    Q_PROPERTY(int  gapY          READ gapY          NOTIFY layoutChanged)
    Q_PROPERTY(int  margin        READ margin        NOTIFY layoutChanged)
    Q_PROPERTY(int  stepX         READ stepX         NOTIFY layoutChanged)
    Q_PROPERTY(int  stepY         READ stepY         NOTIFY layoutChanged)
    Q_PROPERTY(bool showDebugGrid    READ showDebugGrid    NOTIFY layoutChanged)
    Q_PROPERTY(bool snapFramesToGrid READ snapFramesToGrid NOTIFY layoutChanged)

public:
    static DesktopConfigBridge & instance();

    int  cellW()         const;
    int  cellH()         const;
    int  gapX()          const;
    int  gapY()          const;
    int  margin()        const;
    int  stepX()         const;
    int  stepY()         const;
    bool showDebugGrid()    const;
    bool snapFramesToGrid() const;

signals:
    void layoutChanged();

private:
    explicit DesktopConfigBridge(QObject * parent = nullptr);
    static DesktopConfigBridge * _instance;
};

} // namespace Gates
