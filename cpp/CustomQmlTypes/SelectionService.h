#pragma once
#include <QObject>
#include <QString>

class SelectionService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString selectedPath READ selectedPath NOTIFY selectedPathChanged)

public:
    static SelectionService & instance();

    Q_INVOKABLE void select(const QString & path);
    Q_INVOKABLE void deselect();

    const QString & selectedPath() const { return _path; }

signals:
    void selectedPathChanged();

private:
    SelectionService() = default;
    QString _path;
    static SelectionService * _inst;
};
