#pragma once
#include <QColor>
#include <QQuickItem>

struct IExplorerBrowser;
class  ShellViewEventSink; // defined in ShellFolderView.cpp — keeps Windows headers out of here

namespace Gates {

// NOTE: This class is NOT used in production.
//
// Embedding IExplorerBrowser as a child HWND inside a Qt QML window does not
// support transparent backgrounds. When a ListView with CLR_NONE asks its
// parent to paint the background via WM_ERASEBKGND, Qt's QQuickWindow ignores
// the message (it renders via D3D/GL, not GDI). The result is an opaque white
// rectangle that cannot be blended with the QML Acrylic blur behind the frame.
//
// This mechanism WOULD work in a pure Win32 host window, where WM_ERASEBKGND
// properly paints the DWM Acrylic surface. In Qt, the only alternatives are:
//   (a) offscreen capture via PrintWindow + composite as QSGTexture (expensive)
//   (b) custom QML icon renderer using Shell APIs — chosen approach in Gates
//
// Kept for reference; do not instantiate.
// class ShellFolderView : public QQuickItem
class ShellFolderView : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString folderPath     READ folderPath     WRITE setFolderPath     NOTIFY folderPathChanged)
    Q_PROPERTY(QColor  backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)

public:
    explicit ShellFolderView(QQuickItem *parent = nullptr);
    ~ShellFolderView() override;

    QString folderPath() const;
    void    setFolderPath(const QString &path);

    QColor backgroundColor() const;
    void   setBackgroundColor(const QColor &color);

signals:
    void folderPathChanged();
    void backgroundColorChanged();

protected:
    void componentComplete() override;
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    void itemChange(ItemChange change, const ItemChangeData &data) override;

private:
    void tryInit();
    void initBrowser();
    void navigateTo(const QString &path);
    void updateBrowserRect();
    void destroyBrowser();

    IExplorerBrowser  *_browser       = nullptr;
    ShellViewEventSink *_events       = nullptr;
    quint32             _eventsCookie = 0;
    QString             _path;
    QColor              _bgColor      {18, 18, 28}; // dark default, set from QML
    bool                _ready        = false;
};

} // namespace Gates
