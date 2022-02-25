#ifndef GRAPH_VIEW_H
#define GRAPH_VIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QQueue>
#include <QFile>
#include <QTextStream>
#include <QScrollBar>
#include <QLabel>

//Header for MyVex class
#include <QGraphicsEllipseItem>
#include <QVector>

//Headers for lines
#include <QPainter>
#include <QColor>
#include <QGraphicsPathItem>

#include <QTimeLine>
#include <QEasingCurve>

#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
#include <QtMath>
#endif

class viewLog : public QLabel{
    Q_OBJECT
private:
    QFont logFont = QFont("Corbel", 12);
    QString logText;
    void resizeEvent(QResizeEvent *event);
public:
    viewLog(QString log, QWidget *parent = nullptr);
};

class MyGraphicsView;
class MyGraphicsVexItem;
class MyGraphicsLineItem;


// Summary:
//  MyGraphicsView class is customized for visualizing graph
//
// Functions:
//  -Left click to add vex
//  -Left click on vex to add arc
//  -Right click on vex or arc to view information
//  -Drag vex to adjust place (adjust connected arcs either)

class MyGraphicsView : public QGraphicsView{
    Q_OBJECT

private:
    enum mouseStates{
        NORMAL      = 0b00000000,
        ON_HOVER    = 0b00010000,
        ON_SELECTED = 0b00100000,
        ON_MOVING   = 0b01000000
    };

    enum itemStates{
        NON         = 0b00000000,
        SEL         = 0b00010000,
        ADD         = 0b00100000,
        VEX         = 0b00000001,
        LINE        = 0b00000010
    };

    QGraphicsScene* myGraphicsScene;
    int type;
    int vexID = 0;

    int getIdOf(MyGraphicsVexItem* vex){return vexes.indexOf(vex);}

    int mouseState = NORMAL;
    int itemState = NON;
    bool onRightPress = false;

    //For dragging and scaling
    bool onMiddlePress = false;
    QPointF lastPos;

    QGraphicsItem *selItem = nullptr;

    MyGraphicsVexItem *strtVex = nullptr;
    QGraphicsItem *sketchItem = nullptr;
    qreal zValue = -1;

    /* Animation loop */
    QQueue<QTimeLine*> aniQueue;
    bool onAni = false;
    QTimeLine *curAni = nullptr;
    qreal speedRate = 1;
    void nextAni();

    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *event){this->setSceneRect(this->rect());}

    void changeCursor();

    MyGraphicsVexItem* addVex(QPointF center, qreal radius = 10);
    void clearSketch();
    void sketchLine(QPointF start, QPointF end);
    void addLine(MyGraphicsVexItem* start, MyGraphicsVexItem* end);

public:
    enum { UDG = 128, DG = 256 };
    int vexNum = 0;
    int arcNum = 0;
    /* For Saving */
    QVector<MyGraphicsVexItem*> vexes;
    QVector<MyGraphicsLineItem*> lines;

    /* for visit flag */
    bool hasVisitedItem = false;

    MyGraphicsView(int _type = UDG, QWidget *parent = nullptr);

    MyGraphicsVexItem* selectedVex();
    MyGraphicsLineItem* selectedArc();

    void RemoveVex(MyGraphicsVexItem *vex);
    void RemoveArc(MyGraphicsLineItem *line);
    void HideUnvisited();
    void ShowUnvisited();

    void setAniRate(qreal rate){speedRate = rate;}
    void setType(int _type){type = _type;}
    void unSelect(){itemState = NON;selItem = nullptr;emit unselected();}

    void SaveToFile(QTextStream &ts);
    void ReadFromFile(QTextStream &ts);

signals:
    void mouseMoved(QPointF position);
    void mouseLeftClicked(QPointF position);
    void mouseRightClicked(QPointF position);
    void mouseReleased();

    void vexAdded(MyGraphicsVexItem *vex);
    void arcAdded(MyGraphicsLineItem *arc);
    void logAdded(viewLog *log);
    void selected(QGraphicsItem *item);
    void unselected();
    void visitClear();

public slots:
    void setHover(bool in = true);
    void setSel(QGraphicsItem *sel);
    void startLine(MyGraphicsVexItem* startVex);
    void setMenu(QGraphicsItem *target, bool display = true);
    void addLog(viewLog *log){emit logAdded(log);}
    void vexRemoved(MyGraphicsVexItem* vex){vexes.erase(vexes.begin() + vexes.indexOf(vex));vexNum--;}
    void arcRemoved(MyGraphicsLineItem* line){lines.erase(lines.begin() + lines.indexOf(line));arcNum--;}

    void addAnimation(QTimeLine *ani);

};


// Summary:
//  MyGraphicsVexItem realize the interactions with vex

class MyGraphicsVexItem : public QObject, public QGraphicsEllipseItem{
    Q_OBJECT

private:
    enum {
        PREPARING       = 0b10000000,
        UNDEFINED       = 0b00000000,
        ON_HOVER        = 0b00000001,
        ON_LEFT_CLICK   = 0b00000010,
        ON_RIGHT_CLICK  = 0b00000100,
        ON_SELECTED     = 0b00001000,
        ON_LINING       = 0b00010000,
        ON_MENU         = 0b00100000,
        ON_VISIT        = 0b01000000,
        ON_ACCESS       = 0b10000000
    };

private:
    static unsigned int internalID;
    QBrush regBrush = QBrush(QColor(58, 143, 192));
    QBrush selBrush = QBrush(QColor(208, 90, 110));
    QBrush visitedBrush = QBrush(QColor(0, 137, 108));
    QBrush accessBrush = QBrush(QColor(152, 109, 178));

    QPointF center;
    qreal radius;
    int state = UNDEFINED;
    QTimeLine* curAnimation = nullptr;

    QVector<MyGraphicsLineItem*> linesStartWith;
    QVector<MyGraphicsLineItem*> linesEndWith;

    /* For display temporary tag */
    QGraphicsSimpleTextItem *nameTag = nullptr;
    QString nameText = "";
    QFont nameFont = QFont("Corbel", 13, QFont::Normal, true);
    QGraphicsSimpleTextItem *tag = nullptr;
    QString hintText = "";
    QFont hintFont = QFont("Corbel", 12);

    void displayText();

    void hoverInEffect();
    void hoverOutEffect();
    void onClickEffect();
    void onReleaseEffect();
    void startAnimation();
    void stopAnimation();

    void move(QPointF position);

public:
    enum { Type = UserType + 1 };
    int id;
    MyGraphicsVexItem(QPointF _center, qreal _r, int nameID = 0, QGraphicsItem *parent = nullptr);

    /* initializing */
    void estConnection(MyGraphicsView* view);
    void showAnimation();

    void select();
    void visit(bool visited = true);
    void access(const QString &hint = "", bool isAccess = true);
    void itemHide();
    void itemShow();
    QString Text(){return nameText;}
    void setText(const QString & text){nameTag->setText(text);nameText = text;}
    void addStartLine(MyGraphicsLineItem *line){linesStartWith.push_back(line);}
    void removeStartLine(MyGraphicsLineItem *line){linesStartWith.remove(linesStartWith.indexOf(line));}
    void addEndLine(MyGraphicsLineItem *line){linesEndWith.push_back(line);}
    void removeEndLine(MyGraphicsLineItem *line){linesEndWith.remove(linesEndWith.indexOf(line));}
    void remove();

    bool equalTo(MyGraphicsVexItem *v){return id == v->id;}
    int type() const override {return Type;}
    bool isVisited(){return state & ON_VISIT;}
    qreal getRadius() {return radius;}
    QString getData(){return QString::asprintf("%g %g %g\n", center.x(), center.y(), radius)+nameText;}

signals:
    void setHover(bool in = true);
    void selected(QGraphicsItem *sel);
    void lineFrom(MyGraphicsVexItem *start);
    void menuStateChanged(QGraphicsItem *item, bool display = true);
    void logAdded(viewLog *log);
    void removed(MyGraphicsVexItem *vex);

    void addAnimation(QTimeLine *ani);

public slots:
    void onMouseMove(QPointF position);
    void onLeftClick(QPointF position);
    void onRightClick(QPointF position);
    void onMouseRelease();

};

class MyGraphicsLineItem : public QObject, public QGraphicsLineItem{
    Q_OBJECT

private:
    enum {
        UNDEFINED       = 0b00000000,
        ON_HOVER        = 0b00000001,
        ON_LEFT_CLICK   = 0b00000010,
        ON_RIGHT_CLICK  = 0b00000100,
        ON_SELECTED     = 0b00001000,
        ON_MENU         = 0b00100000,
        ON_VISIT        = 0b01000000
    };

    /* basic data */
    bool hasDirection;
    MyGraphicsVexItem *startVex;
    MyGraphicsVexItem *endVex;
    QGraphicsLineItem *line1 = nullptr;
    QGraphicsLineItem *line2 = nullptr;
    QGraphicsPathItem *arrow = nullptr;
    QGraphicsSimpleTextItem *textItem = nullptr;
    QString text = "";

    int state = UNDEFINED;

    /* about animation */
    QTimeLine *curAnimation;

    /* detail of the line */
    qreal lineWidth = 3;
    qreal arrowLength = 10;
    Qt::PenStyle lineStyle = Qt::SolidLine;
    Qt::PenCapStyle capStyle = Qt::RoundCap;
    QColor defaultColor = QColor(125, 185, 222);
    QColor hoverColor = QColor(0, 98, 132);
    QColor selColor = QColor(208, 90, 110);
    QColor visitColor = QColor(0, 137, 108);
    QColor accessColor = QColor(178, 143, 206);
    QPen defaultPen;
    QPen curPen;
    QFont textFont = QFont("Corbel", 12);
    QColor textColor = QColor(0, 0, 0);

    /* for calculation and line rendering */
    qreal angle = 0;
    QPointF center;
    QPointF sP, eP, dP;

    void setLengthRate(qreal r=1);
    void drawLine();
    void drawText();
    void drawArrow();
    void delArrow();

    /* effects */
    void hoverInEffect();
    void hoverOutEffect();
    void onClickEffect();
    void onReleaseEffect();
    void onSelectEffect();
    void deSelectEffect();

public:
    enum { Type = UserType + 2 };
    MyGraphicsLineItem(MyGraphicsVexItem *start, MyGraphicsVexItem *end, bool hasDir = false, QGraphicsItem *parent = nullptr);

    /* initialize functions */
    void estConnection(MyGraphicsView *view);
    void refrshLine();

    /* adjust functions */
    void reverseDirection();
    void moveStart(MyGraphicsVexItem *start);
    void moveEnd(MyGraphicsVexItem *end);
    void setText(const QString & _text);
    void setDirection(bool hasDir = true);

    /* effects */
    //void startAnimation(){}
    //void stopAnimation(){}

    /* retrieve */
    MyGraphicsVexItem* stVex(){return startVex;}
    MyGraphicsVexItem* edVex(){return endVex;}
    QString weightText(){return text;}

    void visit(bool visit = true);
    void itemHide();
    void itemShow();
    void remove();
    void access();

    int type() const override {return Type;}
    bool isVisited(){return state & ON_VISIT;}

signals:
    void setHover(bool in = true);
    void selected(QGraphicsItem *sel);
    void menuStateChanged(QGraphicsItem *item, bool display = true);
    void logAdded(viewLog *log);
    void removed(MyGraphicsLineItem *line);

    void addAnimation(QTimeLine *ani);

private slots:
    void onMouseMove(QPointF position);
    void onLeftClick(QPointF position);
    void onRightClick(QPointF position);
    void onMouseRelease();

};

#endif // GRAPH_VIEW_H
