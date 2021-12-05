#ifndef CUSTOMSCROLLCONTAINER_H
#define CUSTOMSCROLLCONTAINER_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QGraphicsOpacityEffect>
#include <QVector>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer>

#define MAXSPEED 70

class ScrollAreaCustom;
class ScrollListContainer;
class ScrollIndicator;

class ScrollAreaCustom : public QWidget
{
    Q_OBJECT

private:
    QTimer* getCord;
    QTimer* rfrshView;

    ScrollListContainer* container;
    ScrollIndicator* indicator;

    QPropertyAnimation* bounce;

    bool pressed = false;
    bool scrollDown = true;
    bool outOfEdge = false;
    bool ignoreMaxSpeed = false;

    int strtY;
    int lastY;
    int bfEdgeY;    //last y value before out of edge

    int curSpd = 0;
    int damp = 1;
    int moveStored = 0;
    int nextMove = 1;

    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void bounceBack();


public:
    explicit ScrollAreaCustom(QWidget *parent = nullptr);
    void addWidget(QWidget* newWidget, bool setAnimation = true);
    void addWidgets(QVector<QWidget*> widgets){for(int i = 0; i < widgets.size(); i++)addWidget(widgets[i], false);}
    void removeWidget(QWidget* w  = nullptr);
    void scrollToTop();
    void updateHeight();
    void clear();

signals:

private slots:
    void scrollContainer();
    void updateSpd();
    void scrollIndicator(int dp);

};

class ScrollListContainer : public QWidget
{
    Q_OBJECT
public:
    explicit ScrollListContainer(QWidget *parent = nullptr);
    void AddWidget(QWidget* widget, bool setAnimation = true);
    void RemoveWidget(QWidget* widget = nullptr);
    void updateHeight();
    void clear();
    //void RemoveWidget(QWidget* widget);

private:
    //QTimer* newWidgetFade;
    int spacing = 3;
    QVector<QWidget*> widgets;
    int size = 0;
    QVector<int> ys;

    void paintEvent(QPaintEvent* event);

signals:

private slots:

};

class ScrollIndicator : public QWidget
{
    Q_OBJECT

private:
    QColor curColor;
    QColor defaultColor = QColor(100, 100, 100, 130);
    QColor hoverColor = QColor(70, 70, 70, 150);
    QColor pressColor = QColor(50, 50, 50, 170);

    QTimer* hovTimer;
    QTimer* aniPause;

    int lastY;

    int defaultWidth = 2;
    int defaultWidthAtFocus = 9;
    int margin = 3;

    bool pressed = false;

public:
    explicit ScrollIndicator(QWidget *parent = nullptr);

private:
    void paintEvent(QPaintEvent* event);
    void enterEvent(QEnterEvent* event);
    void leaveEvent(QEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

signals:
    void scrollPage(int);

private slots:
    void setHoverActive();

};



#endif // CUSTOMSCROLLCONTAINER_H
