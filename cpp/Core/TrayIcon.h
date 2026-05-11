#pragma once
#include <QObject>
#include <QSystemTrayIcon>

class QMenu;

namespace Gates {

class SettingsController;

class TrayIcon : public QObject
{
    Q_OBJECT

public:
    explicit TrayIcon(SettingsController * settings, QObject * parent = nullptr);

private:
    void onActivated(QSystemTrayIcon::ActivationReason reason);

    QSystemTrayIcon    * _tray     = nullptr;
    QMenu              * _menu     = nullptr;
    SettingsController * _settings = nullptr;
};

} // namespace Gates
