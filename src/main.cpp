#include <iostream>

#include <QApplication>

#include "_basic_frame.h"
#include "frameshandler.h"
#include "_icon.h"





int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


//    Icon test;
    BasicFrame frame;
    frame.add_icon();

    frame.show();
//    frame.animateOpen();
//    FramesHandler handler;
//    handler.makeNewFrame();
//    handler.makeNewFrame();
//    handler.makeNewFrame();
//    handler.showAllFrames();
//    frame.animate();

    return a.exec();
}
