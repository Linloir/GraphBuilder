#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QGraphicsDropShadowEffect>
#include "slidepage.h"
#include "visual_graph/mycanvas.h"
#include "visual_sort/mysort.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    int cornerRadius = 20;
    QWidget *border = nullptr;
    QWidget *defaultPage;
    QGraphicsDropShadowEffect *windowShadow;
    QColor mainBackGround = QColor(251, 251, 251);

    QLineEdit *canvasTitle = nullptr;
    QLineEdit *canvasDesc = nullptr;
    customIcon *settingsIcon = nullptr;
    customIcon *layersIcon = nullptr;
    QWidget *canvasDisplay = nullptr;

    QVector<SlidePage*> pageList;
    SlidePage *graphNewPage = nullptr;
    SlidePage * sortNewPage = nullptr;
    SlidePage *defaultSettingsPage = nullptr;
    SlidePage *curSettingsPage = nullptr;
    SlidePage *allLayers = nullptr; // 图层按钮页
    SlidePage *sortLayerPage = nullptr;
    singleSelectGroup *layerSel = nullptr;

    QVector<MyCanvas*> graphList;
    QVector<MySort*> sortList;
    MyCanvas *curGraph = nullptr;
    MySort *curSort = nullptr;

    void selectGraph(MyCanvas *canvas);
    void deleteCanvas(MyCanvas *canvas);
    void selectSort(MySort *st);
    void deleteSort(MySort *st);
    void selectLayout(QWidget *lyt);
    void selectLayout_(QWidget * x);
    void Init();

    enum {AT_LEFT = 1, AT_TOP = 2,  AT_RIGHT = 4, AT_BOTTOM = 8,
          AT_TOP_LEFT = 3, AT_TOP_RIGHT = 6, AT_BOTTOM_LEFT = 9, AT_BOTTOM_RIGHT = 12};
    bool mousePressed = false;
    int mouseState;
    QPoint lastPos;
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event){
        mousePressed = false;
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
        if(event->globalPosition().y() < 2)
            controlWindowScale();
#else
        if(event->globalPos().y() < 2)
            controlWindowScale();
#endif
    }
    void resizeEvent(QResizeEvent *event);

    bool maximized = false;
    QRect lastGeometry;
    void controlWindowScale();

    MyCanvas* loadCanvas(const QString &path);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
