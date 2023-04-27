#ifndef SHELLQT_H
#define SHELLQT_H

#include <Shlobj.h>
#include <QString>
#include <QStringList>
#include <QIcon>
#include <QPixmap>
#include <QtWinExtras>

void Execute(QString file, QStringList params = QStringList());
QIcon ExtractIcons(QString filepath);

#endif // SHELLQT_H
