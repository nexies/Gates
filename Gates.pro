QT       += core gui
QT       += winextras
#QT       += KWindowSystem

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
MOC_DIR = moc
TARGET = gates
DESTDIR = lib

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/contextmenu.cpp \
    src/desktopframe.cpp \
    src/frame.cpp \
    src/frameshandler.cpp \
    src/icon.cpp \
    src/iconview.cpp \
    src/main.cpp \
    utilities/QtIconProviderExample.cpp \
    utilities/shellqt.cpp

HEADERS += \
    src/contextmenu.h \
    src/desktopframe.h \
    src/frame.h \
    src/frameshandler.h \
    src/icon.h \
    src/iconview.h \
    utilities/shellqt.h

CONFIG += lrelease
CONFIG += embed_translations

INCLUDEPATH += utilities

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
