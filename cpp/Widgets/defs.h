#pragma once
#include <QDebug>

#define MYPROPERTY1 0x10
#define MYPROPERTY2 0x20
#define MYPROPERTY3 0x40

#define DEBUG(...) qDebug() << #__VA_ARGS__; \
qDebug() << "Message is:" << #__VA_ARGS__;





