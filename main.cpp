#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStandardPaths>

#include "cpp/Config/ConfigManager.h"
#include "cpp/Core/GatesFrameDispatcher.h"
#include "cpp/Core/TrayIcon.h"
#include "cpp/Settings/SettingsController.h"
#include "cpp/Shell/DesktopShellManager.h"
#include "cpp/Shell/DesktopLayer.h"

static QString createTestFolder()
{
    const QString path = QDir::temp().absoluteFilePath(QStringLiteral("Gates_test"));
    QDir().mkpath(path);

    // Dummy files — created only if missing so manual edits survive restarts.
    static const char * names[] = {
        "Report.docx", "Budget.xlsx", "Presentation.pptx",
        "Photo.jpg",   "Wallpaper.png", "Clip.mp4",
        "Archive.zip", "Source.cpp",    "Config.json",
        "Notes.txt",   "Readme.md",     "Music.mp3",
        nullptr
    };
    for (int i = 0; names[i]; ++i) {
        const QString fp = QDir(path).absoluteFilePath(QLatin1String(names[i]));
        if (!QFile::exists(fp)) {
            QFile f(fp);
            f.open(QIODevice::WriteOnly);
        }
    }
    return path;
}

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

    const QString testFolder = createTestFolder();

    if (!loaded && config.isFirstLaunch()) {
        qDebug() << "[main] first launch — capturing native desktop icon positions";

        // Capture positions before hiding so we can recreate the layout
        Gates::DesktopShellManager::instance().captureIconPositionsToConfig();

        // Create a default frame pointing at the test folder
        Gates::FrameConfig defaultFrame;
        defaultFrame.id   = Gates::ConfigManager::newFrameId();
        defaultFrame.name = QStringLiteral("My Frame");
        defaultFrame.dir  = testFolder;
        defaultFrame.x    = 80;
        defaultFrame.y    = 80;
        config.config().frames.append(defaultFrame);
        config.save();
    } else {
        // For existing configs that predate the dir field, patch the first frame.
        for (auto & f : config.config().frames) {
            if (f.dir.isEmpty()) {
                f.dir = testFolder;
                break;
            }
        }
        config.save();
    }

    // ── Shell layer ───────────────────────────────────────────────────────────
    Gates::DesktopShellManager::instance().hideNativeIcons();

    // ── Desktop layer (VirtualDesktop + z-order management) ──────────────────
    Gates::DesktopLayer::instance().init();

    // ── Frame dispatcher ──────────────────────────────────────────────────────
    Gates::FrameDispatcher dispatcher;
    dispatcher.init();

    // ── Settings + tray icon ──────────────────────────────────────────────────
    Gates::SettingsController settingsController;
    Gates::TrayIcon           trayIcon(&settingsController);

    return app.exec();
}
