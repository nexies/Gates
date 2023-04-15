#include <iostream>

#include <QApplication>

#include "_basic_frame.h"
#include "frameshandler.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    FramesHandler handler;
    handler.makeNewFrame();
    handler.makeNewFrame();
    handler.makeNewFrame();
    handler.showAllFrames();
//    frame.animate();

    return a.exec();
}
