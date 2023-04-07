#pragma once
#include "SortObject.h"
#include <QVector>
#include <QStack>
#include <QEventLoop>
#include <QVariantAnimation>
#include <algorithm>
#include <cmath>
#include <QtMath>
#include <QTime>
#include <QPainter>
#include <QPaintEvent>
#include <QScopeGuard>
#include <QFontMetrics>
#include <QDebug>
#include <random>

//快速排序
class QuickSort : public SortObject
{
    Q_OBJECT
public:
    explicit QuickSort(QObject *parent = nullptr);

    //开始排序
    //count元素个数，interval动画持续时间参考值
    void sort(int count, int interval) override;
    //结束排序
    void stop() override;
    //绘制
    void draw(QPainter *painter, int width, int height) override;

private:
    void initArr(int count);
    //快速排序，挖坑法
    void doSort(int low, int high);
    //延时
    void wait(int ms);
    //换坑
    void change(int from, int to);

private:
    QEventLoop loop;
    QVariantAnimation animation;
    int interval{0};

    QVector<int> arr;
    //递归范围
    QStack<QPair<int,int>> rangeStack;
    //当前分治的范围
    int rangeBegin{0};
    int rangeEnd{0};
    int curBegin{0};
    int curEnd{0};
    //坑的位置和坑的值
    int posIndex{0};
    int posValue{0};
    //交换
    int swapFrom{0};
    int swapTo{0};
    //标记当前交换状态
    bool swapFlag{false};
};
