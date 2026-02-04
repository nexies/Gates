#pragma once
#include <QQmlApplicationEngine>

class QmlEngine
{
    QmlEngine();

    static QQmlApplicationEngine * _instance;
    static void init();
public:
    static QQmlApplicationEngine & instance();

};
