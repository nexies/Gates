#include "FileIconProvider.h"
#include <QDebug>
#include <QElapsedTimer>

#include <qsystemdetection.h>
#include <QImageReader>
#include <QByteArray>

#ifdef Q_OS_WIN
#include "cpp/utilities/win/shellqt.h"
#endif

FileIconProvider::FileIconProvider()
{

}

QIcon FileIconProvider::icon(const QFileInfo &info) const
{
    QByteArray imageFormat = QImageReader::imageFormat(info.filePath());

    if(!imageFormat.isEmpty())
    {
        qDebug() << "[FileIconProvider] : file " << info.fileName() << "seems to be an image";
        return extractImageIcon(info);
    }

    return QFileIconProvider::icon(info);
}

QIcon FileIconProvider::extractImageIcon(const QFileInfo &info) const
{
    QImageReader reader;
    reader.setFileName(info.filePath());
    QIcon stdIcon = QFileIconProvider::icon(info);

    QImage image = reader.read();
    QPixmap pixmap = QPixmap::fromImage(image);
    // qDebug() << "[FileIconProvider] : resulting pixmap size : " << pixmap.size();

    QIcon out;
    QElapsedTimer timer;

    timer.start();
    out.addPixmap(stdIcon.pixmap({32, 32}));
    out.addPixmap(pixmap.scaled(128, 128, Qt::KeepAspectRatio, Qt::FastTransformation));
    out.addPixmap(pixmap.scaled(512, 512, Qt::KeepAspectRatio, Qt::FastTransformation));

    // qDebug() << "[FileIconProvider] : Pixmap scalings took " << timer.elapsed() << " milliseconds";
    // qDebug() << "                                          " << timer.nsecsElapsed() << " nanoseconds";

    return out;
}
