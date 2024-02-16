#ifndef DIRVIEW_H
#define DIRVIEW_H

#include <QTableView>
#include <QTreeView>
#include <QListView>
#include <QWidget>

#include "dirviewitemdelegate.h"

class QFileSystemModel;
class QFile;

class DirView : public QListView
{
    Q_OBJECT

public:
    DirView(QWidget * parent = nullptr);
    DirView(const QString & path, QWidget * parent = nullptr);
    DirView(const QFile & file, QWidget * parent = nullptr);

    bool setDirectory(const QString & path);
    bool setDirectory(const QFile & file);

    QFileSystemModel * fileSystemModel;
    DirViewItemDelegate * delegate;

protected slots:
    void onActivated(const QModelIndex &);
protected:
//    void paintEvent(QPaintEvent *) override;
};

#endif // DIRVIEW_H
