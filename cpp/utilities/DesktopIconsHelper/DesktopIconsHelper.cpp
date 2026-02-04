#include "DesktopIconsHelper.h"
#include <QDebug>

#ifdef Q_OS_WIN32
#include <ShlObj.h>     // Shell API
// #include <atlcomcli.h>  // CComPtr & Co.
#include <windows.h>
#endif

void DesktopIconsHelper::toggleDesktopIconsHidden(bool hidden)
{
#ifndef Q_OS_WIN32
    qDebug() << "[DesktopIconsHelper][toggleDesktopIconsHidden] : only available in Windows platfom!";
    return;
#else
    // CComPtr<IFolderView2> spView;
    IFolderView2 * spView;
    // FindDesktopFolderView( IID_PPV_ARGS(&spView), "IFolderView2" );
#endif
}
