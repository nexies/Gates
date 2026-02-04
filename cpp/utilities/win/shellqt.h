#ifndef SHELLQT_H
#define SHELLQT_H

#include <QString>
#include <QStringList>
#include <QIcon>
#include <QPixmap>


void EnableBlurBehind();
void Execute(QString file, QStringList params = QStringList());
QIcon extractIcons(QString sourceFile);

#endif // SHELLQT_H
