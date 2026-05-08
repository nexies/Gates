#pragma once

#include <QObject>
#include <QHash>
#include <QPoint>
#include "cpp/Config/ConfigManager.h"

class QTimer;
class QWindow;

namespace Gates {

class FrameForeign;

class FrameDispatcher : public QObject
{
    Q_OBJECT

public:
    explicit FrameDispatcher(QObject * parent = nullptr);

    void init();

    FrameForeign  * frameById(const QString & id) const;
    FrameConfig   * createFrame(int x, int y, int w = 380, int h = 280);
    void            destroyFrame(const QString & id);

    // Returns the frame ID whose window contains globalPos, or empty string.
    QString frameIdAtPos(const QPoint & globalPos) const;

private slots:
    void onConfigChanged();
    void onFrameAdded(Gates::FrameConfig cfg);
    void onFrameRemoved(QString id);
    void onFrameGeometryChanged(QString id, int x, int y, int w, int h);
    void onFrameCollapsedChanged(QString id, bool collapsed);
    void onDropOnFrame(QString path, QString sourceFrameId, QString targetFrameId);
    void onDropOnDesktop(QString path, QString sourceFrameId, qreal x, qreal y);

private:
    void syncFromConfig();
    void spawnFrame(const FrameConfig & cfg);
    void despawnFrame(const QString & id);

    QHash<QString, FrameForeign *> _frames;
    QTimer                        *_saveDebounce = nullptr;
    QWindow                       *_ghostWindow  = nullptr;
};

} // namespace Gates
