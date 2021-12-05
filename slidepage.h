#ifndef SLIDEPAGE_H
#define SLIDEPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPaintEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QGraphicsEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include "customScrollContainer.h"
#include "customWidgets.h"

class SheildLayer : public QWidget{
    Q_OBJECT

private:
    bool pressed = false;
    bool enabled = true;
    QWidget *bg;
    void mousePressEvent(QMouseEvent *event){if(enabled)pressed = true;}
    void mouseReleaseEvent(QMouseEvent *event){if(enabled && pressed)emit clicked();pressed = false;}
    void resizeEvent(QResizeEvent *event){bg->resize(this->parentWidget()->size());}
public:
    SheildLayer(QWidget *parent = nullptr) : QWidget(parent){
        bg = new QWidget(this);
        bg->resize(parent->size());
        bg->setStyleSheet("background-color:#5a000000");
        bg->setAttribute(Qt::WA_TransparentForMouseEvents);
        bg->show();
    }
    void setEnabled(bool e){enabled = e;}
signals:
    void clicked();
};

class SlidePage : public QWidget
{
    Q_OBJECT
private:
    int cornerRadius;
    QString pageName;
    ScrollAreaCustom *pageContentContainer;
    QLabel *nameLabel;
    customIcon *backIcon;
    SheildLayer *sheildLayer;
    QWidget *bgWidget;
    QFont textFont = QFont("Corbel Light", 24);

    bool onShown = false;
    QParallelAnimationGroup *curAni = nullptr;
    QGraphicsOpacityEffect *opacity;

    void resizeEvent(QResizeEvent *event);

public:
    const int preferWidth = 350;
    explicit SlidePage(int radius, QString name, QWidget *parent = nullptr);
    void SetRadius(int radius);
    void SetName(QString name);
    void AddContent(QWidget* widget){widget->setParent(this);pageContentContainer->addWidget(widget, false);}
    void AddContents(QVector<QWidget*> widgets){pageContentContainer->addWidgets(widgets);}
    void RemoveContents(QVector<QWidget*> widgets){for(int i = 0; i < widgets.size(); i++)pageContentContainer->removeWidget(widgets[i]);}
    void UpdateContents(){pageContentContainer->updateHeight();}
    void ScrollToTop(){pageContentContainer->scrollToTop();}

signals:
    void sizeChange();

public slots:
    void slideIn();
    void slideOut();

};

#endif // SLIDEPAGE_H
