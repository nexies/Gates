#ifndef GATESFRAMEDISPATCHER_H
#define GATESFRAMEDISPATCHER_H

#include <QList>
#include <QObject>

class GatesFrameForeign;

class GatesFrameDispatcher : public QObject
{
    Q_OBJECT

public:
    explicit GatesFrameDispatcher (QObject * parent = nullptr);

    bool init ();
    QList<GatesFrameForeign *> _frames;
};

#endif // GATESFRAMEDISPATCHER_H
