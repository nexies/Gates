#include "DesktopFolderLocator.h"

#include <QDir>
#include <QStandardPaths>

#ifdef Q_OS_WIN
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#  include <shlobj.h>
#endif

namespace Gates {

QStringList DesktopFolderLocator::sourceDirs()
{
    QStringList dirs;

    // Per-user Desktop — Qt reads from the Known Folder API, so OneDrive
    // Desktop redirects are handled transparently.
    for (const QString &p : QStandardPaths::standardLocations(QStandardPaths::DesktopLocation))
        if (QDir(p).exists() && !dirs.contains(p))
            dirs.append(p);

#ifdef Q_OS_WIN
    // Public Desktop (%PUBLIC%\Desktop) — always present; QStandardPaths doesn't expose it.
    wchar_t buf[MAX_PATH] = {};
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_COMMON_DESKTOPDIRECTORY, nullptr, 0, buf))) {
        const QString pub = QDir::fromNativeSeparators(QString::fromWCharArray(buf));
        if (QDir(pub).exists() && !dirs.contains(pub))
            dirs.append(pub);
    }
#endif

    return dirs;
}

} // namespace Gates
