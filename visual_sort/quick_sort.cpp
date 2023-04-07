#include "visual_sort/quick_sort.h"


QuickSort::QuickSort(QObject *parent)
    : SortObject(parent)
{
    //属性动画控制交换动画效果
    //animation.setDuration(2000);
    animation.setStartValue(0.0);
    animation.setEndValue(1.0);
    animation.setEasingCurve(QEasingCurve::OutQuart);
    animation.setLoopCount(1);
    connect(&animation, &QVariantAnimation::finished, &loop, &QEventLoop::quit);
    connect(&animation, &QVariantAnimation::valueChanged, this, &SortObject::updateRequest);
}

/*//快速排序挖坑法
template<typename T>
void quick_sort(std::vector<T>& arr, int low, int high)
{
    if (low >= high) {
        return;
    }
    int begin = low;
    int end = high;
    int key = arr[begin];
    while (begin < end)
    {
        while (begin < end && arr[end] >= key) {
            end--;
        }
        if (begin < end) {
            arr[begin] = arr[end];
        }
        while (begin < end && arr[begin] <= key) {
            begin++;
        }
        if (begin < end) {
            arr[end] = arr[begin];
        }
    }
    arr[begin] = key;
    quick_sort(arr, low, begin - 1);
    quick_sort(arr, begin + 1, high);
}*/

void QuickSort::sort(int count, int interval)
{
    auto guard = qScopeGuard([this]{
        setRunFlag(false);
        emit updateRequest();
    });
    Q_UNUSED(guard)

    stop();
    initArr(count);
    setRunFlag(true);

    this->interval = interval;
    doSort(0, arr.size()-1);
}

void QuickSort::stop()
{
    setRunFlag(false);
    animation.stop();
    loop.quit();
    emit updateRequest();
}

void QuickSort::draw(QPainter *painter, int width, int height)
{
    painter->setPen(QColor(200, 200, 200));
    const int len = arr.length();
    //边框距离
    const int left_space = 0;
    const int right_space = 0;
    const int top_space = 30;
    const int bottom_space = 90;

    const int item_space = 10; //柱子横项间隔
    const int text_height = painter->fontMetrics().height();
    const int text_space = 15; //文字和柱子间隔
    const double item_width = (width + item_space - left_space - right_space) / (double)len - item_space;
    const double item_bottom = height - bottom_space;
    const double height_factor = (height - top_space - bottom_space) / (double)len;
    double bottom_offset = 0;
    double item_left = 0;
    double item_height = 0;
    QColor color;
    QString text;
    for (int i = 0; i < len; i++)
    {
        //色块位置x
        item_left = left_space + i * (item_width + item_space);
        item_height = height_factor * arr.at(i);
        bottom_offset = 0;
        //色块颜色
        color = QColor(200, 200, 200);
        text = QString::number(arr.at(i));
        //在执行排序操作的时候标记比较的两个元素
        if (getRunFlag()) {
            //当前排序区域
            if (i >= rangeBegin && i <= rangeEnd) {
                color = QColor(0, 170, 255);
                if (i >= curEnd || i <= curBegin) {
                    if (arr.at(i) < posValue) {
                        color = QColor(0, 220, 0);
                    } else {
                        color = QColor(230, 0, 0);
                    }
                }
            }
            //当前坑的位置，值
            if (i == posIndex) {
                color = QColor(255, 170, 0);
                text = QString("[%1]").arg(QString::number(posValue));
                item_height = height_factor * posValue;
                bottom_offset = 10;
            }
            if (swapFlag) {
                int offset = animation.currentValue().toDouble() *
                        std::abs(swapFrom - swapTo) * (item_width + item_space);
                if (swapFrom < swapTo) {
                    offset = -offset;
                }
                if (i == swapFrom) {
                    item_left -= offset;
                } else if (i == swapTo) {
                    item_left += offset;
                }
            }
        }
        //画文字
        painter->drawText(item_left, item_bottom + text_height + text_space, text);
        //画色块柱子
        painter->fillRect(item_left, item_bottom - item_height - bottom_offset,
                          item_width, item_height,
                          color);
    }

    if (getRunFlag()) {
        //只显示最近的几次递归范围
        const int level = rangeStack.size() > 6 ? rangeStack.size() - 6 : 0;
        int y_pos = height - 5;
        for (int i = level; i < rangeStack.size(); i++, y_pos -= 5)
        {
            QPair<int, int> range = rangeStack.at(i);
            int begin_Left = left_space + range.first * (item_width + item_space);
            int end_right = left_space + range.second * (item_width + item_space) + item_width;
            painter->drawLine(begin_Left, y_pos, end_right, y_pos);
        }
    }

    //文本描述
    painter->fillRect(10, 12, 10, 10, QColor(0, 170, 255));
    painter->drawText(30, 20, "排序区间");
    painter->fillRect(10, 32, 10, 10, QColor(255, 170, 0));
    painter->drawText(30, 40, "坑位");
    painter->fillRect(10, 52, 10, 10, QColor(0, 220, 0));
    painter->drawText(30, 60, "< 参照值");
    painter->fillRect(10, 72, 10, 10, QColor(230, 0, 0));
    painter->drawText(30, 80, "> 参照值");
}

void QuickSort::initArr(int count)
{
    if (count < 2) {
        return;
    }

    //初始化并随机打乱数据
    arr.resize(count);
    for (int i = 0; i < count; i++)
    {
        arr[i] = i + 1;
    }
    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(arr.begin(), arr.end(), g);
//    std::random_shuffle(arr.begin(), arr.end());

    swapFlag = false;
    rangeStack.clear();
    emit updateRequest();
}

void QuickSort::doSort(int low, int high)
{
    if (low >= high) {
        return;
    }
    rangeBegin = low;
    rangeEnd = high;
    curBegin = low;
    curEnd = high;
    rangeStack.push_back({low, high});
    //找一个参照，大于和小于的分别放两边，再对两组数递归执行该操作
    posIndex = curBegin;
    posValue = arr[posIndex]; //挖坑
    wait(interval);
    if (!getRunFlag()) {
        return;
    }
    while (curBegin < curEnd)
    {
        //end从后往前找一个比key小的
        while (curBegin < curEnd && arr[curEnd] >= posValue) {
            wait(interval);
            curEnd--;
            if (!getRunFlag()) {
                return;
            }
            emit updateRequest();
        }
        //比key小的成为新坑，原来较大的值放到右侧那堆数去了
        if (curBegin < curEnd) {
            change(curEnd, curBegin);
            if (!getRunFlag()) {
                return;
            }
        }
        //begin从前往后找一个比key大的
        while (curBegin < curEnd && arr[curBegin] <= posValue) {
            wait(interval);
            curBegin++;
            if (!getRunFlag()) {
                return;
            }
            emit updateRequest();
        }
        //比key大的成为新坑，原来较小的值放到左侧那堆数去了
        if (curBegin < curEnd) {
            change(curBegin, curEnd);
            if (!getRunFlag()) {
                return;
            }
        }
    }
    arr[curBegin] = posValue; //填坑
    wait(interval);
    if (!getRunFlag()) {
        return;
    }
    doSort(low, curBegin - 1);
    if (!getRunFlag()) {
        return;
    }
    doSort(curBegin + 1, high);
    if (!getRunFlag()) {
        return;
    }
    rangeStack.pop();
}

void QuickSort::wait(int ms)
{
    animation.setDuration(ms);
    animation.start();
    loop.exec();
}

void QuickSort::change(int from, int to)
{
    animation.setDuration(interval * 3);
    animation.start();
    swapFlag = true;
    swapFrom = from;
    swapTo = to;
    loop.exec();
    if (getRunFlag()) {
        posIndex = from;
        arr[to] = arr[from];
        swapFlag = false;
    }
}
