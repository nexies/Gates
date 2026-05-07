#pragma once

#include <QObject>
#include <QQmlEngine>

class GatesFrameState : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("GatesFrameState provides enums only")
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
