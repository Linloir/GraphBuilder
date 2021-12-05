#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QGraphicsDropShadowEffect>
#include "slidepage.h"
#include "mycanvas.h"

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
    SlidePage *createNewPage = nullptr;
    SlidePage *defaultSettingsPage = nullptr;
    SlidePage *curSettingsPage = nullptr;
    SlidePage *layersPage = nullptr;
    singleSelectGroup *layerSel = nullptr;

    QVector<MyCanvas*> canvasList;
    MyCanvas *curCanvas = nullptr;

    void selectCanvas(MyCanvas *canvas);
    void deleteCanvas(MyCanvas *canvas);
    void Init();

    enum {AT_LEFT = 1, AT_TOP = 2,  AT_RIGHT = 4, AT_BOTTOM = 8,
          AT_TOP_LEFT = 3, AT_TOP_RIGHT = 6, AT_BOTTOM_LEFT = 9, AT_BOTTOM_RIGHT = 12};
    bool mousePressed = false;
    int mouseState;
    QPoint lastPos;
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event){mousePressed = false;if(event->globalPosition().y() < 2)  controlWindowScale();}
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
