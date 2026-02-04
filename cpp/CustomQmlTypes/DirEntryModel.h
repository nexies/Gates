#pragma once

#include <QStandardItemModel>
#include <QQuickItem>


class QFileIconProvider;

class DirEntryModel : public QStandardItemModel
{
    Q_OBJECT

    Q_PROPERTY(QString currentDirPath READ currentDirPath WRITE setCurDirPath)

    enum Roles
    {
        FileNameRole = Qt::UserRole + 1,
        FilePathRole = Qt::UserRole + 2,
        FilePermissionsRole = Qt::UserRole + 3,
        FileTimeRole = Qt::UserRole + 4,
    };

public:
    explicit DirEntryModel(QObject * parent = nullptr);

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;


    QString currentDirPath() const;
    bool setCurDirPath(const QString &newCurrentDirPath);
private:
    QString m_currentDirPath;
    QFileIconProvider * _iconProvider { nullptr };
};
