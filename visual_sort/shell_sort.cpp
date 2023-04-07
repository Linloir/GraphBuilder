#include "visual_sort/shell_sort.h"


ShellSort::ShellSort(QObject *parent)
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

/*//希尔排序
template<typename T>
void shell_sort(std::vector<T>& arr)
{
    for (int gap = arr.size() / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < arr.size(); i++) {
            int j = i;
            while (j - gap >= 0 && arr[j] < arr[j - gap]) {
                std::swap(arr[j], arr[j - gap]);
                j -= gap;
            }
        }
    }
}*/

void ShellSort::sort(int count, int interval)
{
    auto guard = qScopeGuard([this]{
        setRunFlag(false);
        emit updateRequest();
    });
    Q_UNUSED(guard)

    stop();
    initArr(count);
    setRunFlag(true);

    int len = arr.length();
    //先将待排序的数组元素分成多个序列，然后对每个子序列分别进行直接插入排序
    for (arrGap = len / 2; arrGap > 0; arrGap /= 2)
    {
        for (arrI = arrGap; arrI < len; arrI++)
        {
            arrJ = arrI;
            animation.setDuration(interval);
            animation.start();
            loop.exec();
            emit updateRequest();
            if (!getRunFlag()) {
                return;
            }
            while (arrJ - arrGap >= 0 && arr[arrJ] < arr[arrJ - arrGap])
            {
                animation.setDuration(interval * 3);
                animation.start();
                swapFlag = true;
                loop.exec();
                if (getRunFlag()) {
                    qSwap(arr[arrJ], arr[arrJ - arrGap]);
                    swapFlag = false;
                }
                if (!getRunFlag()) {
                    return;
                }

                arrJ -= arrGap;
                animation.setDuration(interval);
                animation.start();
                loop.exec();
                emit updateRequest();
                if (!getRunFlag()) {
                    return;
                }
            }
            if (!getRunFlag()) {
                return;
            }
        }
        if (!getRunFlag()) {
            return;
        }
    }
}

void ShellSort::stop()
{
    setRunFlag(false);
    animation.stop();
    loop.quit();
    emit updateRequest();
}

void ShellSort::draw(QPainter *painter, int width, int height)
{
    painter->setPen(QColor(200, 200, 200));
    const int len = arr.length();
    //边框距离
    const int left_space = 0;
    const int right_space = 0;
    const int top_space = 20;
    const int bottom_space = 60;

    const int item_space = 10; //柱子横项间隔
    const int text_height = painter->fontMetrics().height();
    const int text_space = 15; //文字和柱子间隔
    const double item_width = (width + item_space - left_space - right_space) / (double)len - item_space;
    const double item_bottom = height - bottom_space;
    const double height_factor = (height - top_space - bottom_space) / (double)len;
    double item_left = 0;
    double item_height = 0;
    QColor color;
    for (int i = 0; i < len; i++)
    {
        //色块位置x
        item_left = left_space + i * (item_width + item_space);
        item_height = height_factor * arr.at(i);
        //色块颜色
        color = QColor(200, 200, 200);
        //在执行排序操作的时候标记比较的两个元素
        if (getRunFlag()) {
            int offset = animation.currentValue().toDouble() * arrGap * (item_width + item_space);
            if (i <= arrI && i % arrGap == arrI % arrGap) {
                color = QColor(0, 170 , 0);
            }
            if (i == arrJ - arrGap && arrJ - arrGap >= 0) {
                color = QColor(0, 170 , 255);
                if (swapFlag) {
                    item_left += offset;
                }
            } else if (i == arrJ) {
                color = QColor(0, 170 , 255);
                if (swapFlag) {
                    item_left -= offset;
                }
            }
        }
        //画文字
        painter->drawText(item_left, item_bottom + text_height + text_space,
                          QString::number(arr.at(i)));
        //画色块柱子
        painter->fillRect(item_left, item_bottom - item_height,
                          item_width, item_height,
                          color);
    }
    //底部画当前的连线
    if (getRunFlag()) {
        painter->setPen(QPen(QColor(255, 170 , 0), 2));
        int y_top = height - 20;
        int y_bottom = height - 8;
        int first_i = arrI % arrGap;
        int last_i = 0;
        for (int i = 0; i < len; i++)
        {
            //色块位置x
            item_left = left_space + i * (item_width + item_space) + item_width / 2;
            if (i % arrGap == arrI % arrGap) {
                last_i = i;
                painter->drawLine(item_left, y_top, item_left, y_bottom);
            }
        }
        int left = left_space + first_i * (item_width + item_space) + item_width / 2;
        int right = left_space + last_i * (item_width + item_space) + item_width / 2;
        painter->drawLine(left, y_bottom, right, y_bottom);

        //标记i位置
        item_left = left_space + arrI * (item_width + item_space);
        const double loop_top = height - 24;
        QPainterPath m_path;
        m_path.moveTo(item_left + item_width / 2.0, loop_top);
        m_path.lineTo(item_left, loop_top + item_width / 2.0);
        m_path.lineTo(item_left + item_width, loop_top + item_width / 2.0);
        m_path.lineTo(item_left + item_width / 2.0, loop_top);
        painter->fillPath(m_path, QColor(0, 255, 0));
    }
}

void ShellSort::initArr(int count)
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

    arrI = 0;
    arrJ = 0;
    arrGap = 0;
    swapFlag = false;
    emit updateRequest();
}
