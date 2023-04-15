#include "frameshandler.h"


void FramesHandler::makeNewFrame()
{
    this->AllFrames.append(new BasicFrame());
}

void FramesHandler::showAllFrames()
{
//    for(int i = 0; i < AllFrames.size(); i++)
//        AllFrames[i]->show();

    for(BasicFrame * frame: AllFrames)
        frame->show();
}
