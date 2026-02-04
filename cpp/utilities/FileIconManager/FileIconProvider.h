#ifndef FILEICONPROVIDER_H
#define FILEICONPROVIDER_H

#include <QFileIconProvider>

class FileIconProvider : public QFileIconProvider
{
public:
    FileIconProvider();

public:
    virtual QIcon icon(const QFileInfo &info) const override;

private:
    QIcon extractImageIcon(const QFileInfo & info) const;
};

#endif // FILEICONPROVIDER_H
