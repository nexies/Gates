#include "TrayIcon.h"
#include "cpp/Settings/SettingsController.h"

#include <QApplication>
#include <QIcon>
#include <QMenu>

namespace Gates {

TrayIcon::TrayIcon(SettingsController * settings, QObject * parent)
    : QObject(parent)
    , _settings(settings)
{
    _menu = new QMenu();
    _menu->addAction(QStringLiteral("Настройки"), _settings, &SettingsController::show);
    _menu->addSeparator();
    _menu->addAction(QStringLiteral("Выход"), qApp, &QApplication::quit);

    _tray = new QSystemTrayIcon(QIcon(QStringLiteral(":/icons/settings_filled.png")), this);
    _tray->setContextMenu(_menu);
    _tray->setToolTip(QStringLiteral("Gates"));
    _tray->show();

    connect(_tray, &QSystemTrayIcon::activated, this, &TrayIcon::onActivated);
}

void TrayIcon::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick)
        _settings->show();
}

} // namespace Gates
