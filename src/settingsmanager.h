#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QWindow>

class SettingsManager : public QWindow
{

    Q_OBJECT

public:
    SettingsManager() {}

public:
signals:
    void signal_setIconSize(QSize p_size);
};

#endif // SETTINGSMANAGER_H
