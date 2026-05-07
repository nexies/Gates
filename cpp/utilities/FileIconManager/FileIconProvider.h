#pragma once

#include <QFileIconProvider>

class FileIconProvider : public QFileIconProvider
{
public:
    FileIconProvider();

    QIcon icon(const QFileInfo & info) const override;

private:
    QIcon extractImageIcon(const QFileInfo & info) const;
};
