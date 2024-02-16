#include "manager.h"
#include <QDir>
#include <QDebug>

using namespace std;
using namespace Gates;

Manager::Manager() {
/*
 *  1) get screen resolution
 *  2) define all scales   ---   which size each element should be
 *  3) prepare the desktop ---   clear all the original icons, make the working directory
 *  4) connect the methods for changing appearance to every element
 *
*/
}

void Gates::Manager::makeNewFrame(QString filePath, QRect geometry)
{
    if(!QDir(filePath).exists()){
        cerr << "Gates::Manager error: Invalid path. Can't create a Frame" << endl;
        return;
    }

    Frame * newFrame = new Frame(filePath);
    newFrame->setGeometry(geometry);
    allFrames.append(newFrame);

    // @todo: link the delete signal from newFrame to Manager

    newFrame->show();
}

void Gates::Manager::makeNewFrame(QString frameName, QString filePath, QRect geometry)
{
    if(!QDir(filePath).exists()){
        cerr << "Gates::Manager error: Invalid path. Can't create a Frame" << endl;
        return;
    }

    Frame * newFrame = new Frame(frameName, filePath);
    newFrame->setGeometry(geometry);
    allFrames.append(newFrame);

    // @todo: link the delete signal from newFrame to Manager

    newFrame->show();
}
