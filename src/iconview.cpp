#include "iconview.h"

Gates::IconView::IconView(AbstractFrame *parent) : QWidget(parent)
{
    this->setAttribute(Qt::WA_PaintOnScreen);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(Qt::FramelessWindowHint);

}
