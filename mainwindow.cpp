#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainterPath>
#include <QRegion>
#include <QTimer>

#if (QT_VERSION > QT_VERSION_CHECK(6,3,0))
#include <QFileDialog>
#endif


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralwidget->setMouseTracking(true);
#ifdef Q_OS_LINUX
    ui->verticalLayout->setContentsMargins(0, 0, 0, 0);
    cornerRadius = 0;
#endif
    QTimer *t = new QTimer(this);
    connect(t, &QTimer::timeout, this, [=](){Init();});
    t->setSingleShot(true);
    t->start(10);

    connect(ui->adjSizeBtn, &QPushButton::clicked, this, [=](){controlWindowScale();});
}

void MainWindow::Init(){
    /* Create main widget and set mask, style sheet and shadow */
#ifdef Q_OS_LINUX
    QPainterPath path;
    path.addRect(ui->mainWidget->rect());
#else
    QPainterPath path;
    path.addRoundedRect(ui->mainWidget->rect(), cornerRadius - 1, cornerRadius - 1);
#endif
    QRegion mask(path.toFillPolygon().toPolygon());
    ui->mainWidget->setMask(mask);

    QString mainStyle;
    ui->mainWidget->setObjectName("mainWidget");
    mainStyle = "QWidget#mainWidget{background-color:" + mainBackGround.name() + QString::asprintf(";border-radius:%dpx", cornerRadius) + "}";
    ui->mainWidget->setStyleSheet(mainStyle);
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
#ifdef Q_OS_WINDOWS
    windowShadow = new QGraphicsDropShadowEffect(this);
    windowShadow->setBlurRadius(30);
    windowShadow->setColor(QColor(0, 0, 0));
    windowShadow->setOffset(0, 0);
    ui->mainWidget->setGraphicsEffect(windowShadow);
#endif
#endif
    /**********************************************************/

    /* Create border in order to cover the zigzag edge of the region */
#ifdef Q_OS_WINDOWS
    border = new QWidget(this);
    border->move(ui->mainWidget->pos() - QPoint(1, 1));
    border->resize(ui->mainWidget->size() + QSize(2, 2));
    QString borderStyle;
    borderStyle = "background-color:#00FFFFFF;border:1.5px solid #686868; border-radius:" + QString::asprintf("%d",cornerRadius) + "px";
    border->setStyleSheet(borderStyle);
    border->setAttribute(Qt::WA_TransparentForMouseEvents);
    border->show();
#endif
    /*****************************************************************/

    /* Create about page */
    //侧页-单击首页设置
    defaultSettingsPage = new SlidePage(cornerRadius, "ABOUT", ui->mainWidget);
    textInputItem *version = new textInputItem("version", defaultSettingsPage);
    version->setValue("1.3-beta");
    version->setEnabled(false);
    textInputItem *updateDate = new textInputItem("last-upd", defaultSettingsPage);
    updateDate->setValue("2023/4/5 10:14");
    updateDate->setEnabled(false);
    textInputItem *Author = new textInputItem("author", defaultSettingsPage);
    Author->setValue("20 Eyes | Made with love");
    Author->setEnabled(false);
    textInputItem *lic = new textInputItem("lic", defaultSettingsPage);
    lic->setValue("MIT License");
    lic->setEnabled(false);
    textInputItem *GitHub = new textInputItem("git", defaultSettingsPage);
    GitHub->setValue("github.com/chuanlukk/visualization");
    GitHub->setEnabled(false);
    defaultSettingsPage->AddContent(GitHub);
    defaultSettingsPage->AddContent(lic);
    defaultSettingsPage->AddContent(Author);
    defaultSettingsPage->AddContent(updateDate);
    defaultSettingsPage->AddContent(version);
    curSettingsPage = defaultSettingsPage;
    defaultSettingsPage->show();
    pageList.push_back(defaultSettingsPage);

    /************************/

    /* Initialize display area */
    // 标题
    QFont titleFont = QFont("Corbel Light", 24);
    QFontMetrics titleFm(titleFont);
    canvasTitle = new QLineEdit(this);
    canvasTitle->setFont(titleFont);
    canvasTitle->setText("START");
    canvasTitle->setMaxLength(20);
    canvasTitle->setReadOnly(true);
    canvasTitle->setMinimumHeight(titleFm.height());
    canvasTitle->setMaximumWidth(titleFm.size(Qt::TextSingleLine, "START").width() + 10);
    canvasTitle->setStyleSheet("background-color:#00000000;border-style:none;border-width:0px;margin-left:1px;");
    connect(canvasTitle, &QLineEdit::textEdited, canvasTitle, [=](QString text){canvasTitle->setMaximumWidth(titleFm.size(Qt::TextSingleLine, text).width());});

    // 描述
    QFont descFont = QFont("Corbel Light", 12);
    QFontMetrics descFm(descFont);
    canvasDesc = new QLineEdit(this);
    canvasDesc->setFont(descFont);
    canvasDesc->setText("Add your first canvas to start");
    canvasDesc->setMaxLength(128);
    canvasDesc->setReadOnly(true);
    canvasDesc->setMinimumHeight(descFm.lineSpacing());
    canvasDesc->setStyleSheet("background-color:#00000000;border-style:none;border-width:0px;");

    // 图标-设置
    settingsIcon = new customIcon(":/icons/icons/settings.svg", "settings", 5, this);
    settingsIcon->setMinimumHeight(canvasTitle->height() * 0.7);
    settingsIcon->setMaximumWidth(canvasTitle->height() * 0.7);
    connect(settingsIcon, &customIcon::clicked, this, [=](){
        QPropertyAnimation *rotate = new QPropertyAnimation(settingsIcon, "rotationAngle", this);
        rotate->setDuration(750);
        rotate->setStartValue(0);
        rotate->setEndValue(90);
        rotate->setEasingCurve(QEasingCurve::InOutExpo);
        rotate->start();
        curSettingsPage->slideIn();
    });
    // 图标-图层
    layersIcon = new customIcon(":/icons/icons/layers.svg", "layers", 5, this);
    layersIcon->setMinimumHeight(canvasTitle->height() * 0.7);
    layersIcon->setMaximumWidth(canvasTitle->height() * 0.7);

    /* create title */
    // 标题栏布局
    QWidget *titleInnerWidget = new QWidget(this);
    titleInnerWidget->setFixedHeight(canvasTitle->height());
    QHBoxLayout *innerLayout = new QHBoxLayout(titleInnerWidget);
    titleInnerWidget->setLayout(innerLayout);
    innerLayout->setContentsMargins(0, 0, 0, 0);
    innerLayout->setSpacing(10);
    innerLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    innerLayout->addWidget(canvasTitle);
    innerLayout->addWidget(settingsIcon);
    innerLayout->addWidget(layersIcon);

    QWidget *titleWidget = new QWidget(this);
    titleWidget->setMaximumHeight(canvasTitle->height() + canvasDesc->height());
    QVBoxLayout *outerLayout = new QVBoxLayout(titleWidget);
    titleWidget->setLayout(outerLayout);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);
    outerLayout->addWidget(titleInnerWidget);
    outerLayout->addWidget(canvasDesc);

    /* create default page */
    // 首页
    defaultPage = new QWidget(ui->mainWidget);
    defaultPage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    bigIconButton *createNew = new bigIconButton(":/icons/icons/create.png", "Create Graph", 10, this);
    createNew->setScale(0.9);


    // MYSORT
    bigIconButton * sortNew = new bigIconButton(":/icons/icons/sort.png", "Create Sort", 10, this);



    bigIconButton *openFile = new bigIconButton(":/icons/icons/open.png", "Open from file", 10, this);
    connect(openFile, &bigIconButton::clicked, this, [=](){
        QString inputPath = QFileDialog::getOpenFileName(this, tr("Open map"), " ",  tr("Map File(*.map)"));
        if(!inputPath.isEmpty()){
            MyCanvas *newCanvas = loadCanvas(inputPath);
            if(newCanvas != nullptr){
                canvasList.push_back(newCanvas);
                selectionItem *newLayer = new selectionItem(newCanvas->name(), newCanvas->description(), layersPage);
                layerSel->AddItem(newLayer);
                layerSel->SetSelection(newLayer);
                pageList.push_back(newCanvas->settingPage());
                connect(newLayer, &selectionItem::selected, this, [=](){selectCanvas(newCanvas);});
                selectCanvas(newCanvas);
                connect(newCanvas, &MyCanvas::nameChanged, this, [=](QString text){
                    canvasTitle->setText(text);
                    canvasTitle->setMaximumWidth(QFontMetrics(QFont("Corbel Light", 24)).size(Qt::TextSingleLine, canvasTitle->text()).width() + 10);
                    newLayer->setTitle(text);
                });
                connect(newCanvas, &MyCanvas::descChanged, this, [=](QString text){this->canvasDesc->setText(text);newLayer->setDescription(text);});
                connect(newCanvas, &MyCanvas::setDel, this, [=](MyCanvas *c){curSettingsPage->slideOut();deleteCanvas(c);layerSel->RemoveItem(newLayer);});
                createNewPage->slideOut();
            }
        }
    });
    QHBoxLayout *defaultPageLayout = new QHBoxLayout(defaultPage);
    defaultPage->setLayout(defaultPageLayout);
    defaultPageLayout->setContentsMargins(50, 30, 50, 80);
    defaultPageLayout->setSpacing(20);
    defaultPageLayout->addWidget(createNew);
    defaultPageLayout->addWidget(sortNew);
    defaultPageLayout->addWidget(openFile);


    /* create layers page */
    //for add new page
    // 侧页-Create Graph
    textInputItem *graphRename = new textInputItem("Name:",createNewPage);
    graphRename->setValue("Layer_" + QString::asprintf("%d", canvasList.size()));
    textInputItem *graphRedescribe = new textInputItem("Detail:",createNewPage);
    graphRedescribe->setValue("No description");


    //MYSORT
    //侧页-Create Sort
    textInputItem *sortRename = new textInputItem("Name:", sortNewPage);
    sortRename->setValue("Sort_" + QString::asprintf("%d", sortList.size()));
    textInputItem *sortRedescribe = new textInputItem("Detail:",sortNewPage);
    sortRedescribe->setValue("No description");



    //侧页-单击首页图层
    layersPage = new SlidePage(cornerRadius, "LAYERS", ui->mainWidget);
    layersPage->stackUnder(createNewPage);
    connect(layersIcon, &customIcon::clicked, layersPage, &SlidePage::slideIn);
    layerSel = new singleSelectGroup("Layers", layersPage);
    connect(layerSel, &singleSelectGroup::itemChange, layersPage, [=](){layersPage->UpdateContents();});
    textButton *openFileBtn = new textButton("Open file", layersPage);
    connect(openFileBtn, &textButton::clicked, this, [=](){
        QString inputPath = QFileDialog::getOpenFileName(this, tr("Open map"), " ",  tr("Map File(*.map)"));
        if(!inputPath.isEmpty()){
            MyCanvas *newCanvas = loadCanvas(inputPath);
            if(newCanvas != nullptr){
                canvasList.push_back(newCanvas);
                selectionItem *newLayer = new selectionItem(newCanvas->name(), newCanvas->description(), layersPage);
                layerSel->AddItem(newLayer);
                layerSel->SetSelection(newLayer);
                pageList.push_back(newCanvas->settingPage());
                connect(newLayer, &selectionItem::selected, this, [=](){selectCanvas(newCanvas);});
                selectCanvas(newCanvas);
                connect(newCanvas, &MyCanvas::nameChanged, this, [=](QString text){
                    canvasTitle->setText(text);
                    canvasTitle->setMaximumWidth(QFontMetrics(QFont("Corbel Light", 24)).size(Qt::TextSingleLine, canvasTitle->text()).width() + 10);
                    newLayer->setTitle(text);
                });
                connect(newCanvas, &MyCanvas::descChanged, this, [=](QString text){this->canvasDesc->setText(text);newLayer->setDescription(text);});
                connect(newCanvas, &MyCanvas::setDel, this, [=](MyCanvas *c){curSettingsPage->slideOut();deleteCanvas(c);layerSel->RemoveItem(newLayer);});
                createNewPage->slideOut();
            }
        }
    });
    textButton *addNewBtn = new textButton("Create new", layersPage);
    layersPage->AddContent(addNewBtn);
    layersPage->AddContent(openFileBtn);
    layersPage->AddContent(layerSel);
    connect(addNewBtn, &textButton::clicked, this, [=](){graphRename->setValue("Layer_" + QString::asprintf("%d", canvasList.size()));graphRedescribe->setValue("No description");createNewPage->slideIn();});
    layersPage->show();
    pageList.push_back(layersPage);

    /* create add new slide page */
    createNewPage = new SlidePage(cornerRadius, "CREATE CANVAS", ui->mainWidget);
    QLineEdit *canvasName = new QLineEdit(this);
    canvasName->setMaximumHeight(20);
    QLineEdit *canvasDesc = new QLineEdit(this);
    canvasDesc->setMaximumHeight(20);

    QWidget *whiteSpace = new QWidget(createNewPage);
    whiteSpace->setFixedHeight(30);
    singleSelectGroup *structureSel = new singleSelectGroup("Structure",createNewPage);
    selectionItem *item_1 = new selectionItem("AL", "Use adjacent list for canvas", createNewPage);
    selectionItem *item_2 = new selectionItem("AML", "Use multiple adjacent list for canvas", createNewPage);
    structureSel->AddItem(item_1);
    structureSel->AddItem(item_2);
    singleSelectGroup *dirSel = new singleSelectGroup("Mode", createNewPage);
    selectionItem *item_3 = new selectionItem("DG", "Directed graph", createNewPage);
    selectionItem *item_4 = new selectionItem("UDG", "Undirected graph", createNewPage);
    dirSel->AddItem(item_3);
    dirSel->AddItem(item_4);
    textButton *submit = new textButton("Create!", createNewPage);
    connect(submit, &textButton::clicked, this, [=](){
        MyCanvas *newCanvas = new MyCanvas(cornerRadius,
                                           graphRename->value(),
                                           graphRedescribe->value(),
                                           structureSel->value() == 0 ? MyCanvas::AL : MyCanvas::AML,
                                           dirSel->value() == 0 ? MyCanvas::DG : MyCanvas::UDG, ui->mainWidget);
        canvasList.push_back(newCanvas);
        selectionItem *newLayer = new selectionItem(newCanvas->name(), newCanvas->description(), layersPage);
        layerSel->AddItem(newLayer);
        layerSel->SetSelection(newLayer);
        pageList.push_back(newCanvas->settingPage());
        connect(newLayer, &selectionItem::selected, this, [=](){selectCanvas(newCanvas);});
        selectCanvas(newCanvas);
        connect(newCanvas, &MyCanvas::nameChanged, this, [=](QString text){
            canvasTitle->setText(text);
            canvasTitle->setMaximumWidth(QFontMetrics(QFont("Corbel Light", 24)).size(Qt::TextSingleLine, canvasTitle->text()).width() + 10);
            newLayer->setTitle(text);
        });
        connect(newCanvas, &MyCanvas::descChanged, this, [=](QString text){this->canvasDesc->setText(text);newLayer->setDescription(text);});
        connect(newCanvas, &MyCanvas::setDel, this, [=](MyCanvas *c){curSettingsPage->slideOut();deleteCanvas(c);layerSel->RemoveItem(newLayer);});
        createNewPage->slideOut();
    });
    createNewPage->AddContent(submit);
    createNewPage->AddContent(dirSel);
    createNewPage->AddContent(structureSel);
    createNewPage->AddContent(whiteSpace);
    createNewPage->AddContent(graphRedescribe);
    createNewPage->AddContent(graphRename);
    connect(createNew, &bigIconButton::clicked, createNewPage, [=](){graphRename->setValue("Layer_" + QString::asprintf("%d", canvasList.size()));graphRedescribe->setValue("No description");createNewPage->slideIn();});
    createNewPage->show();
    pageList.push_back(createNewPage);

    // MYSORT
    sortNewPage = new SlidePage(cornerRadius, "CREATE SORT",ui->mainWidget);
    QLineEdit * sortName = new QLineEdit(this);
    sortName->setMaximumHeight(20);
    QLineEdit * sortDesc = new QLineEdit(this);
    sortDesc->setMaximumHeight(20);
    QWidget * sortWhiteSpace = new QWidget(sortNewPage);
    sortWhiteSpace->setFixedHeight(30);
    singleSelectGroup * sortSel = new singleSelectGroup("Algorithm", sortNewPage);
    selectionItem * sortItems[5];
    sortItems[0] = new selectionItem("Bubble Sort", "冒泡排序");
    sortItems[1] = new selectionItem("Selection Sort", "选择排序");
    sortItems[2] = new selectionItem("Insertion Sort", "插入排序");
    sortItems[3] = new selectionItem("Shell Sort", "希尔排序");
    sortItems[4] = new selectionItem("Quick Sort", "快速排序");
    for(auto item : sortItems)
        sortSel->AddItem(item);
    textButton * goSort = new textButton("Go to Sort!", sortNewPage);
//    connect()
    sortNewPage->AddContent(goSort);
    sortNewPage->AddContent(sortSel);
    sortNewPage->AddContent(sortWhiteSpace);
    sortNewPage->AddContent(sortRedescribe);
    sortNewPage->AddContent(sortRename);
    connect(sortNew, &bigIconButton::clicked, sortNewPage, [=](){
        sortRename->setValue("Sort_" + QString::asprintf("%d", sortList.size()));
        sortRedescribe->setValue("No description");
        sortNewPage->slideIn();
    });
    sortNewPage->show();
    pageList.push_back(sortNewPage);





    ui->displayLayout->addWidget(titleWidget);
    ui->displayLayout->addWidget(defaultPage);
    ui->displayLayout->setAlignment(Qt::AlignTop);
}

void MainWindow::selectCanvas(MyCanvas *canvas){
    if(!curCanvas){
        ui->displayLayout->removeWidget(defaultPage);
        defaultPage->hide();
        ui->displayLayout->addWidget(canvas);
        canvas->show();
    }
    else{
        ui->displayLayout->removeWidget(curCanvas);
        curCanvas->hide();
        ui->displayLayout->addWidget(canvas);
        canvas->show();
    }
    curCanvas = canvas;
    canvas->settingPage()->setParent(ui->mainWidget);
    curSettingsPage = canvas->settingPage();
    canvasTitle->setText(curCanvas->name());
    canvasTitle->setMaximumWidth(QFontMetrics(QFont("Corbel Light", 24)).size(Qt::TextSingleLine, canvasTitle->text()).width() + 10);
    canvasDesc->setText(curCanvas->description());
}

void MainWindow::deleteCanvas(MyCanvas *canvas){
    int index = canvasList.indexOf(canvas);
    if(index < 0)
        return;
    canvasList.erase(canvasList.begin() + index);
    ui->displayLayout->removeWidget(curCanvas);
    curCanvas->hide();
    if(canvasList.size() > 0){
        selectCanvas(canvasList[0]);
    }
    else{
        ui->displayLayout->addWidget(defaultPage);
        defaultPage->show();
        curCanvas = nullptr;
        canvasTitle->setText("START");
        canvasTitle->setMaximumWidth(QFontMetrics(QFont("Corbel Light", 24)).size(Qt::TextSingleLine, "START").width() + 10);
        canvasDesc->setText("Add your first canvas to start");
        curSettingsPage = defaultSettingsPage;
    }
    pageList.erase(pageList.begin() + pageList.indexOf(canvas->settingPage()));
    delete canvas;
    ui->mainWidget->update();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        mousePressed = true;
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
        lastPos = event->globalPosition().toPoint() - this->frameGeometry().topLeft();
#else
        lastPos = event->globalPos() - this->frameGeometry().topLeft();
#endif
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
    if(event->buttons() == Qt::NoButton)
        mousePressed = false;
    if(!mousePressed){
        mouseState = 0;
        if(!maximized && abs(event->pos().x() - ui->mainWidget->pos().x()) < 5)
            mouseState |= AT_LEFT;
        if(!maximized && abs(event->pos().y() - ui->mainWidget->pos().y()) < 5)
            mouseState |= AT_TOP;
        if(!maximized && abs(event->pos().x() - ui->mainWidget->pos().x() - ui->mainWidget->width()) < 5)
            mouseState |= AT_RIGHT;
        if(!maximized && abs(event->pos().y() - ui->mainWidget->pos().y() - ui->mainWidget->height()) < 5)
            mouseState |= AT_BOTTOM;
        if(mouseState == AT_TOP_LEFT  || mouseState == AT_BOTTOM_RIGHT)
            setCursor(Qt::SizeFDiagCursor);
        else if(mouseState == AT_TOP_RIGHT || mouseState == AT_BOTTOM_LEFT)
            setCursor(Qt::SizeBDiagCursor);
        else if(mouseState & (AT_LEFT | AT_RIGHT))
            setCursor(Qt::SizeHorCursor);
        else if(mouseState & (AT_TOP | AT_BOTTOM))
            setCursor(Qt::SizeVerCursor);
        else
            unsetCursor();
    }
    else{
        if(mouseState == 0){
            if(maximized){
                qreal wRatio = (double)event->pos().x() / (double)ui->mainWidget->width();
                controlWindowScale();
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
                this->move(QPoint(event->globalPosition().x() - ui->mainWidget->width() * wRatio, -30));
#else
                this->move(QPoint(event->globalPos().x() - ui->mainWidget->width() * wRatio, -30));
#endif
                lastPos = QPoint(ui->mainWidget->width() * wRatio, event->pos().y());
            }
            else
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
                this->move(event->globalPosition().toPoint() - lastPos);
#else
                this->move(event->globalPos() - lastPos);
#endif
        }
        else{
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
            QPoint d = event->globalPosition().toPoint() - frameGeometry().topLeft() - lastPos;
#else
            QPoint d = event->globalPos() - frameGeometry().topLeft() - lastPos;
#endif
            if(mouseState & AT_LEFT){
                this->move(this->frameGeometry().x() + d.x(), this->frameGeometry().y());
                this->resize(this->width() - d.x(), this->height());
            }
            if(mouseState & AT_RIGHT){
                this->resize(this->width() + d.x(), this->height());
            }
            if(mouseState & AT_TOP){
                this->move(this->frameGeometry().x(), this->frameGeometry().y() + d.y());
                this->resize(this->width(), this->height() - d.y());
            }
            if(mouseState & AT_BOTTOM){
                this->resize(this->width(), this->height() + d.y());
            }
        }
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
        lastPos = event->globalPosition().toPoint() - this->frameGeometry().topLeft();
#else
        lastPos = event->globalPos() - this->frameGeometry().topLeft();
#endif
    }
}

void MainWindow::resizeEvent(QResizeEvent *event){
    //Resize border
    if(border)
        border->resize(ui->mainWidget->size() + QSize(2, 2));

    //Resize mask
    QPainterPath path;
#ifdef Q_OS_WINDOWS
    path.addRoundedRect(ui->mainWidget->rect(), cornerRadius - 1, cornerRadius - 1);
#else
    path.addRect(ui->mainWidget->rect());
#endif
    QRegion mask(path.toFillPolygon().toPolygon());
    ui->mainWidget->setMask(mask);

    //Resize all pages
    for(int i = 0; i < pageList.size(); i++){
        pageList[i]->resize(ui->mainWidget->width() * 0.4 < pageList[i]->preferWidth ? pageList[i]->preferWidth - 1 : ui->mainWidget->width() * 0.4 - 1, ui->mainWidget->height());
        pageList[i]->resize(pageList[i]->width() + 1, pageList[i]->height());
    }
}

void MainWindow::controlWindowScale(){
#ifdef Q_OS_WINDOWS
    if(!maximized){
        lastGeometry = this->frameGeometry();
        windowShadow->setEnabled(false);
        ui->verticalLayout->setContentsMargins(0, 0, 0, 0);
        border->hide();
        QString mainStyle = "QWidget#mainWidget{background-color:" + mainBackGround.name() + ";border-radius:0px;}";
        ui->mainWidget->setStyleSheet(mainStyle);
        this->showMaximized();
        maximized = true;
        QPainterPath path;
        path.addRect(ui->mainWidget->rect());
        QRegion mask(path.toFillPolygon().toPolygon());
        ui->mainWidget->setMask(mask);
    }
    else{
        ui->verticalLayout->setContentsMargins(30, 30, 30, 30);
        this->showNormal();
        QString mainStyle = "QWidget#mainWidget{background-color:" + mainBackGround.name() + QString::asprintf(";border-radius:%dpx", cornerRadius) + "}";
        ui->mainWidget->setStyleSheet(mainStyle);
        QPainterPath path;
        path.addRoundedRect(ui->mainWidget->rect(), cornerRadius - 1, cornerRadius - 1);
        QRegion mask(path.toFillPolygon().toPolygon());
        ui->mainWidget->setMask(mask);
        border->show();
        windowShadow->setEnabled(true);
        this->resize(lastGeometry.width(), lastGeometry.height());
        this->move(lastGeometry.x(), lastGeometry.y());
        maximized = false;
    }
#endif
}

MyCanvas* MainWindow::loadCanvas(const QString &path){
    QFile input(path);
    input.open(QIODevice::ReadOnly);
    QTextStream ts(&input);
    QString magicString = ts.readLine();
    if(magicString != "VFdGeWFXUnZaekl3TURJd05ESTE=")   return nullptr;
    MyCanvas *newCanvas = new MyCanvas(ts, cornerRadius, ui->mainWidget);
    input.close();
    return newCanvas;
}
