#pragma once
#include <QFileIconProvider>

class FileIconProvider : public QFileIconProvider
{
public:
    FileIconProvider();
    virtual QIcon icon(const QFileInfo & info) const override;
};

