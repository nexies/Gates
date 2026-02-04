#pragma once

#include <QObject>

class GatesFrameState : public QObject
{
    Q_OBJECT
public:
    enum NameBarPosition
    {
        NameBarOnTop = 0,
        NameBarOnBottom,
        NameBarHidden,
    };
    Q_ENUM(NameBarPosition)

    enum DockedState
    {
        NotDocked,
        DockedOnTop,
        DockedOnBottom
    };
    Q_ENUM(DockedState)
};
