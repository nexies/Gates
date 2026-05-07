#pragma once

#include <QObject>
#include <QHash>
#include "cpp/Config/ConfigManager.h"

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

private slots:
    void onConfigChanged();
    void onFrameAdded(Gates::FrameConfig cfg);
    void onFrameRemoved(QString id);
    void onFrameGeometryChanged(QString id, int x, int y, int w, int h);

private:
    void syncFromConfig();
    void spawnFrame(const FrameConfig & cfg);
    void despawnFrame(const QString & id);

    QHash<QString, FrameForeign *> _frames;
};

} // namespace Gates
