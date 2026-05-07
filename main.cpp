#include <QApplication>
#include <QDebug>

#include "cpp/Config/ConfigManager.h"
#include "cpp/Core/GatesFrameDispatcher.h"
#include "cpp/Shell/DesktopShellManager.h"
#include "cpp/Shell/DesktopLayer.h"

// Restore native desktop icons on crash so the user is never left with a
// blank desktop. Chains to any previously installed handler.
static LPTOP_LEVEL_EXCEPTION_FILTER s_prevExceptionFilter = nullptr;

static LONG WINAPI gatesExceptionFilter(EXCEPTION_POINTERS * ep)
{
    Gates::DesktopShellManager::instance().restoreNativeIcons();
    return s_prevExceptionFilter ? s_prevExceptionFilter(ep) : EXCEPTION_CONTINUE_SEARCH;
}

int main(int argc, char * argv[])
{
    // Install crash guard before anything else
    s_prevExceptionFilter = SetUnhandledExceptionFilter(gatesExceptionFilter);

    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Gates"));
    app.setOrganizationName(QStringLiteral("Gates"));

    // Always restore native icons on clean exit
    QObject::connect(&app, &QApplication::aboutToQuit, []() {
        Gates::DesktopShellManager::instance().restoreNativeIcons();
    });

    // ── Config ────────────────────────────────────────────────────────────────
    auto & config = Gates::ConfigManager::instance();
    const bool loaded = config.load();

    if (!loaded && config.isFirstLaunch()) {
        qDebug() << "[main] first launch — capturing native desktop icon positions";

        // Capture positions before hiding so we can recreate the layout
        Gates::DesktopShellManager::instance().captureIconPositionsToConfig();

        // Create a default empty frame so the user has something to start with
        Gates::FrameConfig defaultFrame;
        defaultFrame.id   = Gates::ConfigManager::newFrameId();
        defaultFrame.name = QStringLiteral("My Frame");
        defaultFrame.x    = 80;
        defaultFrame.y    = 80;
        config.config().frames.append(defaultFrame);
        config.save();
    }

    // ── Shell layer ───────────────────────────────────────────────────────────
    Gates::DesktopShellManager::instance().hideNativeIcons();

    // ── Desktop layer (VirtualDesktop + z-order management) ──────────────────
    Gates::DesktopLayer::instance().init();

    // ── Frame dispatcher ──────────────────────────────────────────────────────
    Gates::FrameDispatcher dispatcher;
    dispatcher.init();

    return app.exec();
}
