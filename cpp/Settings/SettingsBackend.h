#pragma once
#include <QObject>
#include <QVariantList>

namespace Gates {

class SettingsBackend : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantList frames READ frames NOTIFY framesChanged)
    Q_PROPERTY(int  desktopNumCols    READ desktopNumCols    WRITE setDesktopNumCols    NOTIFY desktopLayoutChanged)
    Q_PROPERTY(int  desktopNumRows    READ desktopNumRows    WRITE setDesktopNumRows    NOTIFY desktopLayoutChanged)
    Q_PROPERTY(int  desktopGapX      READ desktopGapX      WRITE setDesktopGapX      NOTIFY desktopLayoutChanged)
    Q_PROPERTY(int  desktopGapY      READ desktopGapY      WRITE setDesktopGapY      NOTIFY desktopLayoutChanged)
    Q_PROPERTY(int  desktopMargin    READ desktopMargin    WRITE setDesktopMargin    NOTIFY desktopLayoutChanged)
    Q_PROPERTY(bool showDebugGrid    READ showDebugGrid    WRITE setShowDebugGrid    NOTIFY desktopLayoutChanged)
    Q_PROPERTY(bool snapFramesToGrid READ snapFramesToGrid WRITE setSnapFramesToGrid NOTIFY desktopLayoutChanged)
    Q_PROPERTY(int  desktopCellW     READ desktopCellW                               NOTIFY desktopLayoutChanged)
    Q_PROPERTY(int  desktopCellH     READ desktopCellH                               NOTIFY desktopLayoutChanged)

public:
    explicit SettingsBackend(QObject * parent = nullptr);

    QVariantList frames() const;

    int  desktopNumCols()  const;
    int  desktopNumRows()  const;
    int  desktopGapX()     const;
    int  desktopGapY()     const;
    int  desktopMargin()   const;
    bool showDebugGrid()    const;
    bool snapFramesToGrid() const;
    int  desktopCellW()    const;
    int  desktopCellH()    const;

    void setDesktopNumCols(int v);
    void setDesktopNumRows(int v);
    void setDesktopGapX(int v);
    void setDesktopGapY(int v);
    void setDesktopMargin(int v);
    void setShowDebugGrid(bool v);
    void setSnapFramesToGrid(bool v);

    Q_INVOKABLE void    addFrame(const QString & name, const QString & dir);
    Q_INVOKABLE void    removeFrame(const QString & id);
    Q_INVOKABLE void    setFrameName(const QString & id, const QString & name);
    Q_INVOKABLE void    setFrameDir(const QString & id, const QString & dir);
    Q_INVOKABLE QString pickDirectory();

signals:
    void framesChanged();
    void desktopLayoutChanged();

private slots:
    void onConfigChanged();
};

} // namespace Gates
