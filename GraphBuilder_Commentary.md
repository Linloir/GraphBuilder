# 从头开始完成GraphBuilder工程

## 实验要求

1. 以邻接多重表为存储结构，实现联通无向图的深度优先和广度优先遍历。以指定的节点为起点，分别输出每种遍历下的节点访问序列和相应生成树的边集。
2. 以邻接表为存储结构，建立深度优先生成树和广度优先生成树，并以树型输出生成树。
3. 测试数据如图：![测试](gb_commentary/testbench.png)

## 分析需求

> 在本节中将分析上述题目要求，大致明确程序所需要完成的模块及功能

由于需要以树型输出生成树，因此最好能够实现**可视化控件**，其需要支持显示图结构

为了便于添加点和线，故可视化控件需要支持**添加及删除点和线**的功能

从测试数据图中看出，输入的数据为带权图，因此对于该控件还需要实现**添加及修改权重**的功能

由于需要进行遍历，为了体现遍历过程，该控件需要实现**创建节点和边的访问动画**功能

在数据结构层面，需要实现**邻接表**和**邻接多重表**两种数据结构

为了避免演示时不同数据结构需要分开建立测试数据，因此需要为两种不同的数据结构实现一个**虚基类**，并使两种不同的结构均从该虚基类继承并添加**转换函数**，实现两种不同数据结构的互相转换

## 程序设计

> 在本节中，将依据上述需求，对各个需要实现的模块进行更进一步的分析，并给出其实现的过程

从上述需求可以看出，实现最基本的功能大致需要将程序分为两个主要部分：

- **可视化控件**：类画板控件，负责处理图中点和线的元素对象的显示、修改等操作，并为数据结构提供功能接口，诸如调用动画等
- **数据结构**：需要使用要求的数据结构类将上述可视化控件中的元素对象组织起来，并调用其提供的接口实现如深度优先遍历等功能

下面将依次完成这两个主要部分

### 可视化控件（画板）

由于需要实现的这个可视化控件的主要功能是**对图元对象进行管理并使它们可视化**。

QT中已有的`QGraphicsView`具有相似的功能，在文档中有如下描述：

> The QGraphicsView class provides a widget for displaying the contents of a QGraphicsScene
>
> QGraphicsView类是一个将QGraphicsScene场景中的内容可视化显示出来的控件

其中，`QGraphicsScene`是管理所有图元的场景，其在文档中的描述如下：

> The QGraphicsScene class provides a surface for managing large number of 2D graphical items
>
> QGraphicsScene类提供了一个管理大量2D图形项的平台

而`QGraphicsScene`所管理的图形项均继承自`QGraphicsItem`，也就是说对于所有继承自`QGraphicsItem`的图形项目，都可以通过`QGraphicsScene`进行管理，并使用`QGraphicsView`来显示

但是由于不论是`QGraphicsView`还是`QGraphicsItem`，所提供的功能都不足以满足本次项目的功能，因此需要从它们继承并完成新类如下：

<!--
由上述`QGraphicsView`、`QGraphicsScene`以及`QGraphicsItem`的关系可以得知，本次需要完成如下新类：
-->

- `MyGraphicsView`：继承自`QGraphicsView`，添加对鼠标操作的判定和响应、动画队列等功能
- `MyGraphicsVexItem`：继承自`QGraphicsItem`，完成点的绘制和对应功能
- `MyGraphicsLineItem`：继承自`QGraphicsItem`，完成线的绘制和对应功能

下面，我将按照最初的设计思路带大家完成这三个类的实现：

#### 首先，绘制一个点

目前为止，我们面对的任务有：

- 设计点对应的`MyGraphicsVexItem`类
- 设计线对应的`MyGraphicsLineItem`类
- 设计视图框架`MyGraphicsView`将点和线联系起来

按照先挑软柿子捏的逻辑，`MyGraphicsVexItem`类应当是最先进行设计的

首先，先不考虑其他一切问题，如果仅仅只需要在视图场景中绘制一个点，由于点实际上的显示效果是一个圆，因此可以使用`QGraphicsEllipseItem`类：

```cpp
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>

/* Declaration of Graphics View and Graphics Scene */
QGraphicsView* myView = new QGraphicsView;
QGraphicsScene* myScene = new QGraphicsScene;
myScene->setSceneRect(-100, -100, 200, 200);
myView->setScene(myScene);

/* Addition of a point */
//Create and set the top left pos of the new point to (-25, -25) and the radius to 25
QGraphicsEllipseItem* newPoint = new QGraphicsEllipseItem(-25, -25, 50, 50);
myScene->addItem(newPoint);
```

这样我们就以`(0,0)`为圆心，`25`为半径绘制了一个圆。但很显然，这不可能是我们想要的效果，我们需要在绘制出一个简单圆形的基础上添加一些特性，比如点的颜色属性、点的选择状态或是点的`Pop out`动画

为了**在画一个圆**的基础上实现这些额外的功能，我们从`QGraphicsEllipseItem`处继承得到`MyGraphicsVexItem`类

```cpp
class MyGraphicsVexItem : public QGraphicsEllipseItem {
    //Constructors, Properties, Methods, etc.
}
```

##### 先别着急：搭建一个测试框架

在开始着手进行类的各种设计之前，为了明确我们到底设计了什么东西出来，我们需要先搭建一个测试效果用的简单程序框架，它不需要任何UI设计，只需要能够测试我们的类就足够了

首先创建一个工程，在`mainwindow.cpp`中添加如下头文件

```cpp
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
```

在`MainWindow`构造函数中添加如下代码：

```cpp
//Set layout for central widget
QVBoxLayout* mainLayout = new QVBoxLayout();
ui->centralWidget->setLayout(mainLayout);

//Add GraphicsView widget
QGraphicsView* testView = new QGraphicsView(ui->centralWidget);
QGraphicsScene* testScene = new QGraphicsScene(ui->centralWidget);
testScene->setSceneRect(-150, -150, 300, 300);
testView->setScene(testScene);

//Add widget to layout
mainLayout->addWidget(testView);
```

编译程序，即可得到一个简单的GraphicsView显示窗口

后续的测试则通过在这个框架上添加或是修改特定的功能来测试效果

##### 迈出第一步：使用参数绘制点

为了能够在新的类里面使用给定的参数画出点，我们需要如下参数：

- `QPointF center` : 几何中心
- `qreal radius` : 半径
- `QBrush regBrush` : 默认画刷

由于考虑到其他方法也可能会访问或是修改这些参数，因此将他们作为类的成员变量。构造函数则在妥善设置和使用这些参数的基础上，结合父类构造函数完成

具体的修改代码如下：

```cpp
private:
    //Geometry properties
    QPointF center;
    qreal radius;

    //Brush properties
    //default color
    QBrush regBrush = QBrush(QColor(58, 143,  192));

public:
    MyGraphicsVexItem(QPointF _center, qreal _radius, QGraphicsItem* parent = nullptr);
```

```cpp
MyGraphicsVexItem::MyGraphicsVexItem(QPointF _center, qreal _radius, QGraphicsItem* parent) :
    QGraphicsEllipseItem(_center.x() - _radius, _center.y() - _radius, 2 * _radius, 2 * _radius, parent),
    center(_center),
    radius(_radius)
{
    this->setPen(Qt::NoPen);
    this->setBrush(regBrush);
}
```

此时我们在测试框架中用代码添加一个点测试效果：

```cpp
//Add a new vex
MyGraphicsVexItem* newVex = new MyGraphicsVexItem(QPointF(0, 0), 10);
testScene->addItem(newVex);
```
