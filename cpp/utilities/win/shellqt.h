#pragma once

#include <QString>
#include <QStringList>
#include <QIcon>

void EnableBlurBehind();
void Execute(QString file, QStringList params = QStringList());

// Extracts shell icons for any path: regular files, .lnk shortcuts,
// and virtual CLSID paths like "::{645FF040-5081-101B-9F08-00AA002F954E}".
QIcon extractIcons(const QString & sourceFile);
