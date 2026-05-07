#pragma once
#include <QQmlApplicationEngine>

namespace Gates {

class QmlEngine
{
    QmlEngine();

    static QQmlApplicationEngine * _instance;
    static void init();

public:
    static QQmlApplicationEngine & instance();
};

} // namespace Gates
