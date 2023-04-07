#ifndef MYSORT_H
#define MYSORT_H

#include <QWidget>
#include <QGridLayout>
#include "slidepage.h"
#include "visual_sort/maincanvas.h"

#if (QT_VERSION > QT_VERSION_CHECK(6,3,0))
#include <QFileDialog>
#endif


class MySort : public QWidget
{
    Q_OBJECT
private:
    QString sortName;
    QString sortDescription;
    int type;

    SlidePage *settings;

    //For display
    QWidget * cntlWidget;
    MainCanvas * sortCanvas;
//    QWidget * sortGround;
    QHBoxLayout * mainLayout;

    void Init();
    void CreateSettings(int r);

public:
    enum sortType {BUBBLE, SELECTION, INSERTION, SHELL, QUICK};

    explicit MySort(int radius, QString name = "", QString desc = "", int _type = 0, QWidget *parent = nullptr);
    MySort(QTextStream &ts, int radius, QWidget *parent = nullptr);
    QString name(){return sortName;}
    QString description(){return sortDescription;}
    SlidePage * settingPage(){return settings;}

signals:
    void nameChanged(QString name);
    void descChanged(QString desc);
    void setDel(MySort * target);

};

#endif // MYSORT_H
