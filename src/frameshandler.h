#ifndef FRAMESHANDLER_H
#define FRAMESHANDLER_H

#include <QObject>
#include <QList>

#include "abstractframe.h"

namespace Gates{

class FramesHandler : public QObject
{
    Q_OBJECT

// constructors
public:
    FramesHandler() {}

    ~FramesHandler() {
        for(auto frame: AllFrames)
            delete frame;
    }

// fields
public:
    QList<AbstractFrame *> AllFrames;

// methods
public:
    void makeNewFrame();
    void showAllFrames();
    void collectAllDesktopFiles();

};

}
#endif // FRAMESHANDLER_H
