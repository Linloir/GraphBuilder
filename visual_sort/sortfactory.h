#ifndef SORTFACTORY_H
#define SORTFACTORY_H

#include <QObject>
#include "visual_sort/sortobject.h"
#include "visual_sort/bubble_sort.h"
#include "visual_sort/selection_sort.h"
#include "visual_sort/insertion_sort.h"
#include "visual_sort/quick_sort.h"
#include "visual_sort/shell_sort.h"

//生成SortObject
class SortFactory : public QObject
{
    Q_OBJECT
private:
    explicit SortFactory(QObject *parent = nullptr);
public:
    static SortFactory * getInstance();

    //创建一个排序对象
    //row对应sortlist列表的排序方式
    SortObject *createSortObject(int row, QObject *parent);

    //返回排序方式列表，作为combobox选项
    QStringList getSortList() const;
signals:

};

#endif // SORTFACTORY_H
