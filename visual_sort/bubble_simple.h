#pragma once
#include <QWidget>
#include <QVector>
#include <QEventLoop>
#include <QVariantAnimation>
#include <algorithm>
#include <cmath>
#include <QtMath>
#include <QTime>
#include <QPushButton>
#include <QPainter>
#include <QPaintEvent>
#include <QScopeGuard>
#include <QDebug>

//使用冒泡排序对动画逻辑进行测试
class BubbleSimple : public QWidget
{
    Q_OBJECT
public:
    explicit BubbleSimple(QWidget *parent = nullptr);

    bool getRunFlag() const;
    void setRunFlag(bool flag);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void initArr();
    void run();
    void stop();

signals:
    void runFlagChanged(bool running);

private:
    //=false时退出for循环
    bool runFlag{false};
    QEventLoop loop;
    QVariantAnimation animation;

    QVector<int> arr;
    //冒泡for循环下标
    int arrI{0};
    int arrJ{0};
    //标记当前交换状态
    bool swapFlag{false};
};
