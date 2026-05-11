#pragma once
#include <QObject>
#include <QScopedPointer>

class QWindow;
class QQmlContext;

namespace Gates {

class SettingsBackend;

class SettingsController : public QObject
{
    Q_OBJECT

public:
    explicit SettingsController(QObject * parent = nullptr);
    ~SettingsController() override;

    void show();
    void hide();
    bool isVisible() const;

private:
    void createWindow();

    SettingsBackend         * _backend = nullptr;
    QQmlContext             * _context = nullptr;
    QScopedPointer<QWindow>   _window;
};

} // namespace Gates
