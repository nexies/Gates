#pragma once

#include <QString>
#include <QStringList>
#include <QIcon>

void EnableBlurBehind();
void Execute(QString file, QStringList params = QStringList());

// Extracts shell icons for any path: regular files, .lnk shortcuts,
// and virtual CLSID paths like "::{645FF040-5081-101B-9F08-00AA002F954E}".
QIcon extractIcons(const QString & sourceFile);

// Resolves a .lnk shortcut's explicit icon location (via IShellLink) and
// returns its icon. More reliable than SHGetFileInfoW when the icon path
// uses environment variables (e.g. Steam shortcuts).
// Returns a null QIcon if the shortcut has no resolvable icon location.
QIcon extractShortcutIcon(const QString & lnkPath);

// Primary icon extraction via IShellItemImageFactory — handles all file types,
// shortcuts (follows them), and returns a pixmap at the requested size.
// Returns a null QPixmap on failure.
QPixmap extractFilePixmap(const QString & path, int size);
