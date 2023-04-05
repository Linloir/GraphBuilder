#ifndef MYSORT_H
#define MYSORT_H

#include <QWidget>
#include <QGridLayout>
#include "slidepage.h"


class MySort : public QWidget
{
    Q_OBJECT
private:
    QString sortName;
    QString sortDescription;

    SlidePage *settings;

    //For display
    QWidget * cntlWidget;
    QHBoxLayout * mainLayout;

public:
//    enum {}

    explicit MySort(QWidget *parent = nullptr);
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
