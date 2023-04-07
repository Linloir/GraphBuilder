#ifndef MAINCANVAS_H
#define MAINCANVAS_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <visual_sort/sortobject.h>

class MainCanvas : public QWidget
{
    Q_OBJECT
public:
    explicit MainCanvas(QWidget *parent = nullptr);

    //不同的SortObejct对应不同排序规则
    int getSortType() const;
    void setSortObject(int type, SortObject *obj);
    //开始排序
    void sort(int count, int interval);
    //结束排序
    void stop();

protected:
    void paintEvent(QPaintEvent *event) override;

signals:
    //排序执行状态
    void runFlagChanged(bool running);

private:
    int sortType{-1};
    SortObject *sortObj{nullptr};
};

#endif // MAINCANVAS_H
