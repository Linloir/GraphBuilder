#ifndef SORTOBJECT_H
#define SORTOBJECT_H

#include <QObject>
#include <QPainter>

//排序对象父类，提供公共接口给canvas
class SortObject : public QObject
{
    Q_OBJECT
public:
    explicit SortObject(QObject *parent = nullptr);

    //开始排序
    //count元素个数，interval动画持续时间参考值
    virtual void sort(int count, int interval) = 0;
    //结束排序
    virtual void stop() = 0;
    //绘制
    virtual void draw(QPainter *painter, int width, int height) = 0;

    //running排序状态
    bool getRunFlag() const;
    void setRunFlag(bool flag);

signals:
    void runFlagChanged(bool running);
    void updateRequest();

private:
    //排序执行状态，=true则正在排序
    bool runFlag{false};
};

#endif // SORTOBJECT_H
