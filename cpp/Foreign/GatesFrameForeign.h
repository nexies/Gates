#pragma once

#include <QObject>
#include <QWindow>
#include "cpp/CustomQmlTypes/DirEntryModel.h"
#include "cpp/Config/ConfigManager.h"

class QQmlContext;

namespace Gates {

class FrameForeign : public QObject
{
    Q_OBJECT

public:
    explicit FrameForeign(const FrameConfig & cfg, QObject * parent = nullptr);

    const QString & frameId() const { return _id; }
    QWindow       * window()  const { return _window.get(); }
    QString         dirPath()  const { return _dirModel ? _dirModel->currentDirPath() : QString(); }

    void applyConfig(const FrameConfig & cfg);

public slots:
    void show();
    void hide();
    void setVisible(bool visible);
    void setDirectory(const QString & directory);
    void setColor(const QColor & color);
    void setOpacity(float opacity);

signals:
    void geometryChanged(QString id, int x, int y, int w, int h);
    void visibilityChanged(QString id, bool visible);
    void collapsedChanged(QString id, bool collapsed);
    void dockedEdgeChanged(QString id, QString edge); // "none"|"top"|"bottom"

private slots:
    void onWindowMinimisedChanged();
    void onWindowDockedStateChanged();

protected:
    QString getNameForDirectory(const QString & path);

private:
    void connectWindowSignals();

    QString                  _id;
    QQmlContext             *_context  { nullptr };
    QScopedPointer<QWindow>  _window   { nullptr };
    DirEntryModel           *_dirModel { nullptr };
};

} // namespace Gates
