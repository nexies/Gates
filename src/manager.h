#ifndef MANAGER_H
#define MANAGER_H

#include <QMainWindow>
#include <QString>
#include <QList>


#include "frame.h"

namespace Gates {

class Manager: public QMainWindow{

    Q_OBJECT

public:
    Manager();
    ~Manager(){
        for(Frame * fr: allFrames)
            fr->~Frame();
    }

public:
    void makeNewFrame(QString frameName, QString filePath, QRect geometry = QRect(0, 0, 700, 400));


public:
    QList<Frame *> allFrames;
};


}

#endif // MANAGER_H
