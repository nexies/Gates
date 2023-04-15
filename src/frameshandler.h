#ifndef FRAMESHANDLER_H
#define FRAMESHANDLER_H

#include <QObject>
#include <QList>

#include "_basic_frame.h"

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
    QList<BasicFrame *> AllFrames;

// methods
public:
    void makeNewFrame();
    void showAllFrames();

};

#endif // FRAMESHANDLER_H
