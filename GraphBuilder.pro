QT       += core gui
QT += svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    customScrollContainer.cpp \
    customWidgets.cpp \
    graph_implement.cpp \
    graph_view.cpp \
    main.cpp \
    mainwindow.cpp \
    mycanvas.cpp \
    slidepage.cpp

HEADERS += \
    customScrollContainer.h \
    customWidgets.h \
    graph_implement.h \
    graph_view.h \
    mainwindow.h \
    mycanvas.h \
    slidepage.h

FORMS += \
    mainwindow.ui

RC_ICONS = logo.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc
