#include "DragDropService.h"

#include <QPoint>
#include <QtMath>

DragDropService & DragDropService::instance()
{
    static DragDropService inst;
    return inst;
}

DragDropService::DragDropService(QObject * parent) : QObject(parent) {}

void DragDropService::startDrag(const QString & sourceFrameId,
                                const QString & path,
                                qreal globalX, qreal globalY)
{
    _sourceFrameId  = sourceFrameId;
    _dragPath       = path;
    _ghostX         = globalX;
    _ghostY         = globalY;
    _hoveredFrameId = sourceFrameId;
    _active         = true;
    emit dragPathChanged();
    emit ghostPosChanged();
    emit hoveredFrameIdChanged();
    emit activeChanged();
}

void DragDropService::updatePos(qreal globalX, qreal globalY)
{
    if (!_active) return;

    _ghostX = globalX;
    _ghostY = globalY;
    emit ghostPosChanged();

    if (_frameLocator) {
        const QString hovered = _frameLocator(QPoint(qRound(globalX), qRound(globalY)));
        if (hovered != _hoveredFrameId) {
            _hoveredFrameId = hovered;
            emit hoveredFrameIdChanged();
        }
    }
}

bool DragDropService::commitDrop(qreal globalX, qreal globalY)
{
    if (!_active) return false;

    const QString path     = _dragPath;
    const QString sourceId = _sourceFrameId;

    _active         = false;
    _dragPath.clear();
    _sourceFrameId.clear();
    _hoveredFrameId.clear();
    emit activeChanged();
    emit dragPathChanged();
    emit hoveredFrameIdChanged();

    QString targetId;
    if (_frameLocator)
        targetId = _frameLocator(QPoint(qRound(globalX), qRound(globalY)));

    if (!targetId.isEmpty()) {
        if (targetId != sourceId)
            emit dropOnFrame(path, sourceId, targetId);
        // same-frame drop: no-op (future: in-grid reorder)
        return true;
    }

    // targetId empty → dropped on desktop background
    emit dropOnDesktop(path, sourceId, globalX, globalY);
    return false;
}

void DragDropService::cancelDrag()
{
    if (!_active) return;
    _active         = false;
    _dragPath.clear();
    _sourceFrameId.clear();
    _hoveredFrameId.clear();
    emit activeChanged();
    emit dragPathChanged();
    emit hoveredFrameIdChanged();
}
