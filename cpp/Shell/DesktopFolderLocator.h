#pragma once
#include <QStringList>

namespace Gates {

// Discovers all filesystem directories whose contents appear on the Windows desktop.
class DesktopFolderLocator
{
public:
    static QStringList sourceDirs();
};

} // namespace Gates
