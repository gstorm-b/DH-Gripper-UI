QT       += core gui serialport serialbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dh-robotic/dh_rgi.cpp \
    dh-robotic/dhcontroller.cpp \
    dh-robotic/dhrgiwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    timer/TimeCounter.cpp \
    widget/serialsettingwidget.cpp

HEADERS += \
    dh-robotic/dh_rgi.h \
    dh-robotic/dhcontroller.h \
    dh-robotic/dhr_define.h \
    dh-robotic/dhrgiwidget.h \
    mainwindow.h \
    timer/TimeCounter.h \
    widget/serialsettingwidget.h

FORMS += \
    dh-robotic/dhrgiwidget.ui \
    mainwindow.ui \
    widget/serialsettingwidget.ui

TRANSLATIONS += \
    dh-gripper-ui_vi_VN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
