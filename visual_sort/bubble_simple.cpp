#include "visual_sort/bubble_simple.h"


BubbleSimple::BubbleSimple(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Simple Bubble Sort (by GongJianBo 1992)");
    resize(800, 600);

    //属性动画控制交换动画效果
    //animation.setDuration(2000);
    animation.setStartValue(0.0);
    animation.setEndValue(1.0);
    animation.setEasingCurve(QEasingCurve::OutQuart);
    animation.setLoopCount(1);
    connect(&animation, &QVariantAnimation::finished, &loop, &QEventLoop::quit);
    connect(&animation, &QVariantAnimation::valueChanged, this, [this]{
        update();
    });

    //点击执行排序
    QPushButton *btn_run = new QPushButton("run", this);
    btn_run->move(10, 10);
    connect(btn_run, &QPushButton::clicked, this, &BubbleSimple::run);

    //点击终止排序
    QPushButton *btn_stop = new QPushButton("stop", this);
    btn_stop->move(10, 60);
    connect(btn_stop, &QPushButton::clicked, this, &BubbleSimple::stop);

    //因为loop是全局的，导致run函数不可重入，否则第一个run还没退出第二个又进去了
    connect(this, &BubbleSimple::runFlagChanged, [=](bool flag){
        btn_run->setEnabled(!flag);
    });

    initArr();
}

bool BubbleSimple::getRunFlag() const
{
    return runFlag;
}

void BubbleSimple::setRunFlag(bool flag)
{
    if (runFlag == flag) {
        return;
    }
    runFlag = flag;
    emit runFlagChanged(flag);
}

void BubbleSimple::paintEvent(QPaintEvent *event)
{
    event->accept();
    QPainter painter(this);
    painter.translate(20, 150);
    int len = arr.length();
    int spacing = 40;
    for (int i = 0; i < len; i++)
    {
        //色块位置x
        int x_offset = i * spacing;
        //色块颜色
        QColor color = QColor(50, 50, 50);
        //在执行排序操作的时候标记比较的两个元素
        if (getRunFlag()) {
            if (i == arrJ) {
                color = QColor(255, 170 , 0);
                if (swapFlag) {
                    x_offset += animation.currentValue().toDouble() * spacing;
                }
            } else if (i == arrJ + 1) {
                color = QColor(0, 170 , 255);
                if (swapFlag) {
                    x_offset -= animation.currentValue().toDouble() * spacing;
                }
            } else if (i >= len - arrI) {
                //已排序好的
                color = QColor(0, 170, 0);
            }
        }
        //画文字
        painter.drawText(x_offset, 0, QString::number(arr.at(i)));
        //画色块柱子
        painter.fillRect(QRect(x_offset, 20, 20, 100 + arr.at(i) * 20), color);
    }
}

void BubbleSimple::initArr()
{
    arr = QVector<int>{8, 5, 6, 3, 1, 9, 4, 2, 7, 10};
    arrI = 0;
    arrJ = 0;
    swapFlag = false;
    update();
}

/*//一般的冒泡排序写法
template<typename T>
void bubble_sort(T arr[], int len)
{
    for (int i = 0; i < len - 1; i++) {
        for (int j = 0; j < len - 1 - i; j++) {
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j],arr[j+1]);
            }
        }
    }
}*/

void BubbleSimple::run()
{
    qDebug()<<"running";
    auto guard = qScopeGuard([this]{
        setRunFlag(false);
        qDebug()<<"finished";
        update();
    });
    Q_UNUSED(guard)

    stop();
    initArr();
    setRunFlag(true);

    int len = arr.length();
    for (arrI = 0; arrI < len - 1; arrI++)
    {
        for (arrJ = 0; arrJ < len - 1 - arrI; arrJ++)
        {
            if (arr[arrJ] > arr[arrJ + 1]) {
                animation.setDuration(1000);
                animation.start();
                swapFlag = true;
                loop.exec();
                if (getRunFlag()) {
                    qSwap(arr[arrJ], arr[arrJ + 1]);
                    swapFlag = false;
                }
            } else {
                animation.setDuration(300);
                animation.start();
                loop.exec();
            }
            update();
            if (!getRunFlag()) {
                return;
            }
        }
        if (!getRunFlag()) {
            return;
        }
    }
}

void BubbleSimple::stop()
{
    setRunFlag(false);
    animation.stop();
    loop.quit();
}
