#include "customScrollContainer.h"

ScrollAreaCustom::ScrollAreaCustom(QWidget *parent) : QWidget(parent)
{
    //initialize list container and timer
    container = new ScrollListContainer(this);
    container->move(0, 0);
    container->resize(this->width(), 3);
    getCord = new QTimer;
    getCord->setSingleShot(true);
    rfrshView = new QTimer;
    getCord->setSingleShot(true);

    indicator = new ScrollIndicator(this);
    indicator->resize(indicator->width(), (int)((double)this->height() * this->height() / (double)container->height()));
    indicator->move(this->width() - indicator->width() - 3, 0);

    this->setMouseTracking(true);
    container->setMouseTracking(true);
    indicator->setMouseTracking(true);

    bounce = new QPropertyAnimation(container, "pos");

    QObject::connect(getCord, SIGNAL(timeout()), this, SLOT(updateSpd()));
    QObject::connect(rfrshView, SIGNAL(timeout()), this, SLOT(scrollContainer()));
    QObject::connect(indicator, SIGNAL(scrollPage(int)), this, SLOT(scrollIndicator(int)));
}

void ScrollAreaCustom::paintEvent(QPaintEvent *event){
    container->resize(this->width(), container->height());
    if(container->height() > this->height() && container->y() < this->height() - container->height() && curSpd == 0 && bounce->state() == QAbstractAnimation::Stopped)
        container->move(container->x(), this->height() - container->height());
    if(container->height() <= this->height()){
        container->move(container->x(), 0);
        indicator->hide();
    }
    else{
        indicator->show();
    }
    indicator->resize(indicator->width(), (int)((double)this->height() * this->height() / (double)container->height()));
    indicator->move(this->width() - indicator->width() - 3, -container->y() * this->height() / container->height());
}

void ScrollAreaCustom::mousePressEvent(QMouseEvent *event){
    if(container->height() > this->height()){
        if(container->y() <= 0 && container->y() + container->height() >= this->height())
            pressed = true;
        lastY = event->pos().y();
    }
    getCord->stop();
    rfrshView->stop();
    curSpd = 0;
    outOfEdge = false;
    moveStored = 0;
    nextMove = 1;
}

void ScrollAreaCustom::mouseMoveEvent(QMouseEvent *event){
    setCursor(Qt::ArrowCursor);
    if(pressed){
        //start scroll
        if(!getCord->isActive() && event->pos().y() - lastY != 0){
            //start 30ms timer
            getCord->start(30);
            strtY = event->pos().y();
        }
        if(container->y() <= 0 && container->y() + container->height() >= this->height())
            container->move(container->x(), container->y() + event->pos().y() - lastY);
        else{
            if(!outOfEdge){
                bfEdgeY = event->pos().y();
                container->move(container->x(), container->y() + event->pos().y() - lastY);
                outOfEdge = true;
            }
            else{
                int pos = container->y() >= 0 ? 1 : -1;
                int dp = event->pos().y() - bfEdgeY;
                if(dp == 0){
                    outOfEdge = false;
                    nextMove = 1;
                    moveStored = 0;
                    if(container->y() >= 0)
                        container->move(container->x(), 0);
                    else
                        container->move(container->x(), this->height() - container->height());
                }
                else if(dp / abs(dp) != pos){
                    outOfEdge = false;
                    container->move(container->x(), this->y() + event->pos().y() - bfEdgeY);
                    nextMove = 1;
                    moveStored = 0;
                }
                else{
                    while(abs(moveStored) + nextMove <= abs(event->pos().y() - bfEdgeY)){
                        moveStored += nextMove * pos;
                        container->move(container->x(), container->y() + pos);
                        nextMove++;
                    }
                    while(nextMove > 1 && abs(moveStored) > abs(event->pos().y() - bfEdgeY)){
                        nextMove--;
                        moveStored -= nextMove * pos;
                        container->move(container->x(), container->y() - pos);
                    }
                    if(moveStored == 0){
                        outOfEdge = false;
                        if(container->y() >= 0)
                            container->move(container->x(), 0);
                        else
                            container->move(container->x(), this->height() - container->height());
                        nextMove = 1;
                        moveStored = 0;
                    }
                }
            }
        }
        lastY = event->pos().y();
    }
}

void ScrollAreaCustom::mouseReleaseEvent(QMouseEvent *event){
    //start scrolling
    if(container->y() > 0 || container->y() + container->height() < this->height())
        bounceBack();
    else
        rfrshView->start(30);
    pressed = false;
}

void ScrollAreaCustom::bounceBack(){
    rfrshView->stop();
    getCord->stop();
    bounce->setDuration(500);
    bounce->setStartValue(container->pos());
    if(container->y() > 0)
        bounce->setEndValue(QPoint(container->x(), 0));
    else
        bounce->setEndValue(QPoint(container->x(), this->height() - container->height()));
    bounce->setEasingCurve(QEasingCurve::OutQuad);
    bounce->start();
}

void ScrollAreaCustom::scrollContainer(){
    //scroll
    if(curSpd > 0){
        if(curSpd > MAXSPEED && !ignoreMaxSpeed)
            curSpd = MAXSPEED;
        else if(curSpd <= MAXSPEED) ignoreMaxSpeed = false;
        int dp = scrollDown ? curSpd : -curSpd;
        container->move(container->x(), container->y() + dp);
    }
    else
        return;
    if(container->y() <= 0 && container->y() + container->height() >= this->height()){
        curSpd -= damp;
        curSpd = curSpd < 0 ? 0 : curSpd;
    }
    else
        curSpd /= 2;
    if(curSpd == 0 && (container->y() > 0 || container->y() + container->height() < this->height()))
        bounceBack();
    else
        rfrshView->start(30);
}

void ScrollAreaCustom::updateSpd(){
    int spd = lastY - strtY;
    scrollDown = spd >= 0;
    strtY = lastY;
    curSpd = abs(spd);
}

void ScrollAreaCustom::addWidget(QWidget *newWidget, bool setAnimation){
    newWidget->setParent(container);
    container->AddWidget(newWidget, setAnimation);
}

void ScrollAreaCustom::removeWidget(QWidget *w){
    container->RemoveWidget(w);
}

void ScrollAreaCustom::scrollToTop(){
    curSpd = sqrt(8 * (- container->pos().y()) + 2) / 2;
    scrollDown = true;
    getCord->stop();
    rfrshView->stop();
    outOfEdge = false;
    moveStored = 0;
    nextMove = 1;
    ignoreMaxSpeed = true;
    rfrshView->start(30);
}

void ScrollAreaCustom::updateHeight(){
    container->updateHeight();
}

void ScrollAreaCustom::clear(){
    container->clear();
}

void ScrollAreaCustom::scrollIndicator(int dp){
    int newY = container->y() - dp * container->height() / this->height();
    if(newY > 0)
        newY = 0;
    else if(newY < this->height() - container->height())
        newY = this->height() - container->height();
    container->move(container->x(), newY);
    update();
}

void ScrollAreaCustom::wheelEvent(QWheelEvent *event){
    if(container->y() > 0 || container->y() + container->height() < this->height())
        return;
    curSpd += 5;
    bool newDirection = event->angleDelta().y() > 0;
    if(newDirection != scrollDown)
        curSpd = 5;
    if(curSpd > MAXSPEED)
        curSpd = MAXSPEED;
    scrollDown = newDirection;
    if(!rfrshView->isActive())
        rfrshView->start(30);
    update();
}

ScrollListContainer::ScrollListContainer(QWidget *parent) : QWidget(parent){}

void ScrollListContainer::paintEvent(QPaintEvent *event){
    for(int i = 0; i < widgets.size(); i++){
        widgets[i]->resize(this->width(), widgets[i]->height());
    }
}

void ScrollListContainer::AddWidget(QWidget *widget, bool setAnimation){
    //Add animation for all widgets current
    this->resize(this->width(), this->height() + widget->height() + spacing);
    widgets.push_back(widget);
    size++;
    ys.push_back(0);
    widget->resize(this->width(), widget->height());
    widget->show();

    if(setAnimation){
        QGraphicsOpacityEffect* widgetOpac = new QGraphicsOpacityEffect(widget);
        widgetOpac->setOpacity(0);
        widget->setGraphicsEffect(widgetOpac);
        QParallelAnimationGroup* dpGroup = new QParallelAnimationGroup;
        QSequentialAnimationGroup* newWidgetFadeIn = new QSequentialAnimationGroup;
        for(int i = 0; i < size - 1; i++){
            ys[i] += widget->height() + spacing;
            QPropertyAnimation* move = new QPropertyAnimation(widgets[i], "pos");
            move->setDuration(750);
            move->setStartValue(widgets[i]->pos());
            move->setEndValue(QPoint(widgets[i]->x(), ys[i]));
            move->setEasingCurve(QEasingCurve::InOutQuart);
            dpGroup->addAnimation(move);
        }
        newWidgetFadeIn->addPause(300);
        QPropertyAnimation* fade = new QPropertyAnimation(widgetOpac, "opacity", widget);
        fade->setDuration(300);
        fade->setStartValue(0);
        fade->setEndValue(0.99);
        newWidgetFadeIn->addAnimation(fade);
        dpGroup->addAnimation(newWidgetFadeIn);
        dpGroup->start();
        connect(dpGroup, &QPropertyAnimation::stateChanged, [=](){
            if(dpGroup->state() == QAbstractAnimation::Stopped){
                if(widgetOpac->opacity() != 0.99){
                    fade->start(QAbstractAnimation::DeleteWhenStopped);
                    connect(fade,&QPropertyAnimation::finished,[=](){widgetOpac->deleteLater();});
                }
                else{
                    dpGroup->deleteLater();
                    widgetOpac->deleteLater();
                }
            }
        });
    }
    else{
        for(int i = 0; i < size - 1; i++){
            ys[i] += widget->height() + spacing;
            widgets[i]->move(QPoint(widgets[i]->pos().x(), ys[i]));
        }
    }
}

void ScrollListContainer::RemoveWidget(QWidget *widget){
    int index;
    if(widget == nullptr){
        index = size - 1;
        if(index != -1)
            widget = widgets[index];
    }
    else
        index = widgets.indexOf(widget);
    if(index == -1 || widget == nullptr){
        return;
    }
    this->resize(this->width(), this->height() - widget->height() - spacing);
    this->parentWidget()->update();
    widget->hide();
    widget->setParent(nullptr);
    QParallelAnimationGroup* dpGroup = new QParallelAnimationGroup;
    for(int i = index - 1; i >= 0; i--){
        ys[i] -= (widget->height() + spacing);
        QPropertyAnimation* move = new QPropertyAnimation(widgets[i], "pos");
        move->setDuration(750);
        move->setStartValue(widgets[i]->pos());
        move->setEndValue(QPoint(widgets[i]->x(), ys[i]));
        move->setEasingCurve(QEasingCurve::InOutQuart);
        dpGroup->addAnimation(move);
    }
    dpGroup->start(QAbstractAnimation::DeleteWhenStopped);
    widgets.remove(index);
    size--;
    ys.remove(index);
}

void ScrollListContainer::updateHeight(){
    for(int i = size - 2; i >= 0; i--){
        ys[i] = ys[i + 1] + widgets[i + 1]->height() + spacing;
        widgets[i]->move(widgets[i]->pos().x(), ys[i]);
    }
    this->resize(this->width(), ys[0] + widgets[0]->height() + 3);
}

void ScrollListContainer::clear(){
    int n = size;
    for(int i = 0; i < n; i++)
        RemoveWidget();
}

ScrollIndicator::ScrollIndicator(QWidget *parent) : QWidget(parent)
{
    this->resize(defaultWidth, 0);
    hovTimer = new QTimer(this);
    hovTimer->setSingleShot(true);
    aniPause = new QTimer(this);
    aniPause->setSingleShot(true);
    QObject::connect(hovTimer, SIGNAL(timeout()), this, SLOT(setHoverActive()));
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->curColor = defaultColor;

    this->setMouseTracking(true);
}

void ScrollIndicator::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(curColor);
    painter.drawRect(this->rect());
}

void ScrollIndicator::enterEvent(QEnterEvent *event){
    if(!pressed){
        hovTimer->start(100);
        curColor = hoverColor;
        update();
    }
}

void ScrollIndicator::leaveEvent(QEvent *event){
    hovTimer->stop();
    curColor = defaultColor;
    QPropertyAnimation* narrow = new QPropertyAnimation(this, "geometry");
    narrow->setDuration(300);
    narrow->setStartValue(QRect(this->x(), this->y(), this->width(), this->height()));
    narrow->setEndValue(QRect(this->parentWidget()->width() - margin - defaultWidth, this->y(), defaultWidth, this->height()));
    narrow->setEasingCurve(QEasingCurve::InOutQuad);
    narrow->start(QAbstractAnimation::DeleteWhenStopped);
    aniPause->start(300);
    update();
}

void ScrollIndicator::mousePressEvent(QMouseEvent *event){
    curColor = pressColor;
    pressed = true;
    //>note: globalPos -> globalPosition here due to deprecation
    //>      may cause issues
    lastY = event->globalPosition().y();
    update();
}

void ScrollIndicator::mouseMoveEvent(QMouseEvent *event){
    if(pressed && !aniPause->isActive()){
        //>note: globalPos -> globalPosition here due to deprecation
        //>      may cause issues
        int dp = event->globalPosition().y() - lastY;
        emit scrollPage(dp);
        //>note: globalPos -> globalPosition here due to deprecation
        //>      may cause issues
        lastY = event->globalPosition().y();
    }
}

void ScrollIndicator::mouseReleaseEvent(QMouseEvent *event){
    pressed = false;
    curColor = hoverColor;
    update();
}

void ScrollIndicator::setHoverActive(){
    QPropertyAnimation* widen = new QPropertyAnimation(this, "geometry");
    widen->setDuration(300);
    widen->setStartValue(QRect(this->x(), this->y(), this->width(), this->height()));
    widen->setEndValue(QRect(this->parentWidget()->width() - margin - defaultWidthAtFocus, this->y(), defaultWidthAtFocus, this->height()));
    widen->setEasingCurve(QEasingCurve::InOutQuad);
    widen->start(QAbstractAnimation::DeleteWhenStopped);
    aniPause->start(300);
}
