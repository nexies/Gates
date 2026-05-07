#pragma once

#include <QObject>
#include <functional>

class DragDropService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool    active        READ active        NOTIFY activeChanged)
    Q_PROPERTY(QString dragPath      READ dragPath      NOTIFY dragPathChanged)
    Q_PROPERTY(QString sourceFrameId READ sourceFrameId NOTIFY activeChanged)
    Q_PROPERTY(QString hoveredFrameId READ hoveredFrameId NOTIFY hoveredFrameIdChanged)
    Q_PROPERTY(qreal   ghostX        READ ghostX        NOTIFY ghostPosChanged)
    Q_PROPERTY(qreal   ghostY        READ ghostY        NOTIFY ghostPosChanged)

public:
    static DragDropService & instance();

    bool    active()         const { return _active; }
    QString dragPath()       const { return _dragPath; }
    QString sourceFrameId()  const { return _sourceFrameId; }
    QString hoveredFrameId() const { return _hoveredFrameId; }
    qreal   ghostX()         const { return _ghostX; }
    qreal   ghostY()         const { return _ghostY; }

    // Called by FrameDispatcher to inject geometry lookup (avoids circular deps)
    using FrameLocator = std::function<QString(QPoint)>;
    void setFrameLocator(FrameLocator fn) { _frameLocator = std::move(fn); }

    // QML-callable interface
    Q_INVOKABLE void startDrag(const QString & sourceFrameId,
                               const QString & path,
                               qreal globalX, qreal globalY);
    Q_INVOKABLE void updatePos(qreal globalX, qreal globalY);

    // Returns true if the drop landed on a frame (caller should NOT update
    // desktop position); false if it landed on the desktop background.
    Q_INVOKABLE bool commitDrop(qreal globalX, qreal globalY);
    Q_INVOKABLE void cancelDrag();

signals:
    void activeChanged();
    void dragPathChanged();
    void ghostPosChanged();
    void hoveredFrameIdChanged();

    // Emitted by commitDrop — FrameDispatcher listens and does the file move
    void dropOnFrame(QString path, QString sourceFrameId, QString targetFrameId);
    void dropOnDesktop(QString path, QString sourceFrameId, qreal x, qreal y);

private:
    explicit DragDropService(QObject * parent = nullptr);

    FrameLocator _frameLocator;

    bool    _active        = false;
    QString _dragPath;
    QString _sourceFrameId;
    QString _hoveredFrameId;
    qreal   _ghostX = 0;
    qreal   _ghostY = 0;
};
