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
    main.cpp \
    mainwindow.cpp \
    slidepage.cpp \
    visual_graph/graph_implement.cpp \
    visual_graph/graph_view.cpp \
    visual_graph/mycanvas.cpp \
    visual_sort/bubble_simple.cpp \
    visual_sort/bubble_sort.cpp \
    visual_sort/insertion_sort.cpp \
    visual_sort/maincanvas.cpp \
    visual_sort/mysort.cpp \
    visual_sort/quick_sort.cpp \
    visual_sort/selection_sort.cpp \
    visual_sort/shell_sort.cpp \
    visual_sort/sortfactory.cpp \
    visual_sort/sortobject.cpp

HEADERS += \
    customScrollContainer.h \
    customWidgets.h \
    mainwindow.h \
    slidepage.h \
    visual_graph/graph_implement.h \
    visual_graph/graph_view.h \
    visual_graph/mycanvas.h \
    visual_sort/bubble_simple.h \
    visual_sort/bubble_sort.h \
    visual_sort/insertion_sort.h \
    visual_sort/maincanvas.h \
    visual_sort/mysort.h \
    visual_sort/quick_sort.h \
    visual_sort/selection_sort.h \
    visual_sort/shell_sort.h \
    visual_sort/sortfactory.h \
    visual_sort/sortobject.h

FORMS += \
    mainwindow.ui

RC_ICONS = logo.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc
