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
                graphList.push_back(newCanvas);
                selectionItem *newLayer = new selectionItem(newCanvas->name(), newCanvas->description(), allLayers);
                layerSel->AddItem(newLayer);
                layerSel->SetSelection(newLayer);
                pageList.push_back(newCanvas->settingPage());
                connect(newLayer, &selectionItem::selected, this, [=](){selectLayer(newCanvas);});
                selectLayer(newCanvas);
                connect(newCanvas, &MyCanvas::nameChanged, this, [=](QString text){
                    canvasTitle->setText(text);
                    canvasTitle->setMaximumWidth(QFontMetrics(QFont("Corbel Light", 24)).size(Qt::TextSingleLine, canvasTitle->text()).width() + 10);
                    newLayer->setTitle(text);
                });
                connect(newCanvas, &MyCanvas::descChanged, this, [=](QString text){this->canvasDesc->setText(text);newLayer->setDescription(text);});
                connect(newCanvas, &MyCanvas::setDel, this, [=](MyCanvas *c){curSettingsPage->slideOut();deleteLayer(c);layerSel->RemoveItem(newLayer);});
                graphNewPage->slideOut();
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
    textInputItem *graphRename = new textInputItem("Name:",graphNewPage);
    graphRename->setValue("Graph_" + QString::asprintf("%d", graphList.size()));
    textInputItem *graphRedescribe = new textInputItem("Detail:",graphNewPage);
    graphRedescribe->setValue("No description");


    //MYSORT
    //侧页-Create Sort
    textInputItem *sortRename = new textInputItem("Name:", sortNewPage);
    sortRename->setValue("Sort_" + QString::asprintf("%d", sortList.size()));
    textInputItem *sortRedescribe = new textInputItem("Detail:",sortNewPage);
    sortRedescribe->setValue("No description");



    //侧页-单击首页图层
    allLayers = new SlidePage(cornerRadius, "LAYERS", ui->mainWidget);
    allLayers->stackUnder(sortNewPage);
    connect(layersIcon, &customIcon::clicked, allLayers, &SlidePage::slideIn);
    layerSel = new singleSelectGroup("Layers", allLayers);
    connect(layerSel, &singleSelectGroup::itemChange, allLayers, [=](){allLayers->UpdateContents();});
    textButton *openFileBtn = new textButton("Open file", allLayers);
    connect(openFileBtn, &textButton::clicked, this, [=](){
        QString inputPath = QFileDialog::getOpenFileName(this, tr("Open map"), " ",  tr("Map File(*.map)"));
        if(!inputPath.isEmpty()){
            MyCanvas *newCanvas = loadCanvas(inputPath);
            if(newCanvas != nullptr){
                graphList.push_back(newCanvas);
                selectionItem *newLayer = new selectionItem(newCanvas->name(), newCanvas->description(), allLayers);
                layerSel->AddItem(newLayer);
                layerSel->SetSelection(newLayer);
                pageList.push_back(newCanvas->settingPage());
                connect(newLayer, &selectionItem::selected, this, [=](){selectLayer(newCanvas);});
                selectLayer(newCanvas);
                connect(newCanvas, &MyCanvas::nameChanged, this, [=](QString text){
                    canvasTitle->setText(text);
                    canvasTitle->setMaximumWidth(QFontMetrics(QFont("Corbel Light", 24)).size(Qt::TextSingleLine, canvasTitle->text()).width() + 10);
                    newLayer->setTitle(text);
                });
                connect(newCanvas, &MyCanvas::descChanged, this, [=](QString text){this->canvasDesc->setText(text);newLayer->setDescription(text);});
                connect(newCanvas, &MyCanvas::setDel, this, [=](MyCanvas *c){curSettingsPage->slideOut();deleteLayer(c);layerSel->RemoveItem(newLayer);});
                graphNewPage->slideOut();
            }
        }
    });
    textButton *addNewGraphBtn = new textButton("Create new Graph", allLayers);
    textButton *addNewSortBtn = new textButton("Create new Sort", allLayers);
    allLayers->AddContent(addNewSortBtn);
    allLayers->AddContent(addNewGraphBtn);
    allLayers->AddContent(openFileBtn);
    allLayers->AddContent(layerSel);
    connect(addNewGraphBtn, &textButton::clicked, this, [=](){graphRename->setValue("Graph_" + QString::asprintf("%d", graphList.size()));graphRedescribe->setValue("No description");graphNewPage->slideIn();});
    connect(addNewSortBtn, &textButton::clicked, this, [=](){sortRename->setValue("Sort_" + QString::asprintf("%d", sortList.size()));sortRedescribe->setValue("No description");sortNewPage->slideIn();});
    allLayers->show();
    pageList.push_back(allLayers);

    /* create add new slide page */
    graphNewPage = new SlidePage(cornerRadius, "CREATE GRAPH", ui->mainWidget);
    QLineEdit *canvasName = new QLineEdit(this);
    canvasName->setMaximumHeight(20);
    QLineEdit *canvasDesc = new QLineEdit(this);
    canvasDesc->setMaximumHeight(20);
    QWidget *whiteSpace = new QWidget(graphNewPage);
    whiteSpace->setFixedHeight(30);
    singleSelectGroup *structureSel = new singleSelectGroup("Structure",graphNewPage);
    selectionItem *item_1 = new selectionItem("AL", "Use adjacent list for canvas", graphNewPage);
    selectionItem *item_2 = new selectionItem("AML", "Use multiple adjacent list for canvas", graphNewPage);
    structureSel->AddItem(item_1);
    structureSel->AddItem(item_2);
    singleSelectGroup *dirSel = new singleSelectGroup("Mode", graphNewPage);
    selectionItem *item_3 = new selectionItem("DG", "Directed graph", graphNewPage);
    selectionItem *item_4 = new selectionItem("UDG", "Undirected graph", graphNewPage);
    dirSel->AddItem(item_3);
    dirSel->AddItem(item_4);
    textButton *submit = new textButton("Create!", graphNewPage);
    connect(submit, &textButton::clicked, this, [=](){
        MyCanvas *newCanvas = new MyCanvas(cornerRadius,
                                           graphRename->value(),
                                           graphRedescribe->value(),
                                           structureSel->value() == 0 ? MyCanvas::AL : MyCanvas::AML,
                                           dirSel->value() == 0 ? MyCanvas::DG : MyCanvas::UDG, ui->mainWidget);
        graphList.push_back(newCanvas);
        selectionItem *newLayer = new selectionItem(newCanvas->name(), newCanvas->description(), allLayers);
        layerSel->AddItem(newLayer);
        layerSel->SetSelection(newLayer);
        pageList.push_back(newCanvas->settingPage());

        // DEBUG
        connect(newLayer, &selectionItem::selected, this, [=](){
            selectLayer(newCanvas);

        });
        selectLayer(newCanvas);


        connect(newCanvas, &MyCanvas::nameChanged, this, [=](QString text){
            canvasTitle->setText(text);
            canvasTitle->setMaximumWidth(QFontMetrics(QFont("Corbel Light", 24)).size(Qt::TextSingleLine, canvasTitle->text()).width() + 10);
            newLayer->setTitle(text);
        });
        connect(newCanvas, &MyCanvas::descChanged, this, [=](QString text){this->canvasDesc->setText(text);newLayer->setDescription(text);});
        connect(newCanvas, &MyCanvas::setDel, this, [=](MyCanvas *c){curSettingsPage->slideOut();deleteLayer(c);layerSel->RemoveItem(newLayer);});
        graphNewPage->slideOut();
    });


    graphNewPage->AddContent(submit);
    graphNewPage->AddContent(dirSel);
    graphNewPage->AddContent(structureSel);
    graphNewPage->AddContent(whiteSpace);
    graphNewPage->AddContent(graphRedescribe);
    graphNewPage->AddContent(graphRename);
    connect(createNew, &bigIconButton::clicked, graphNewPage, [=](){graphRename->setValue("Graph_" + QString::asprintf("%d", graphList.size()));graphRedescribe->setValue("No description");graphNewPage->slideIn();});
    graphNewPage->show();
    pageList.push_back(graphNewPage);

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
    connect(goSort, &textButton::clicked, this, [=](){
        MySort * newSort = new MySort(cornerRadius,
                                      sortRename->value(),
                                      sortRedescribe->value(),
                                      sortSel->value(),
                                      ui->mainWidget);
        sortList.push_back(newSort);
        selectionItem * newLayer = new selectionItem(newSort->name(), newSort->description(), allLayers);
        layerSel->AddItem(newLayer);
        layerSel->SetSelection(newLayer);
        pageList.push_back(newSort->settingPage());
        // DEBUG
        connect(newLayer, &selectionItem::selected, this, [=](){
            selectLayer(newSort);

        });
        qDebug()<<"newSort:"<<newSort;
        selectLayer(newSort);


        connect(newSort, &MySort::nameChanged, this, [=](QString text){
            canvasTitle->setText(text);
            canvasTitle->setMaximumWidth(QFontMetrics(QFont("Corbel Light", 24)).size(Qt::TextSingleLine, canvasTitle->text()).width() + 10);
            newLayer->setTitle(text);
        });
        connect(newSort, &MySort::descChanged, this, [=](QString text){this->canvasDesc->setText(text);newLayer->setDescription(text);});
        connect(newSort, &MySort::setDel, this, [=](MySort * c){curSettingsPage->slideOut();deleteLayer(c);layerSel->RemoveItem(newLayer);});
        sortNewPage->slideOut();
    });



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

/*
void MainWindow::selectGraph(MyCanvas *canvas){
    if(!curGraph){
        ui->displayLayout->removeWidget(defaultPage);
        defaultPage->hide();
        ui->displayLayout->addWidget(canvas);
        canvas->show();
    }
    else{
        ui->displayLayout->removeWidget(curGraph);
        curGraph->hide();
        ui->displayLayout->addWidget(canvas);
        canvas->show();
    }
    curGraph = canvas;
    canvas->settingPage()->setParent(ui->mainWidget);
    curSettingsPage = canvas->settingPage();
    canvasTitle->setText(curGraph->name());
    canvasTitle->setMaximumWidth(QFontMetrics(QFont("Corbel Light", 24)).size(Qt::TextSingleLine, canvasTitle->text()).width() + 10);
    canvasDesc->setText(curGraph->description());
}
*/

/*
void MainWindow::deleteCanvas(MyCanvas *canvas){
    int index = graphList.indexOf(canvas);
    if(index < 0)
        return;
    graphList.erase(graphList.begin() + index);
    ui->displayLayout->removeWidget(curGraph);
    curGraph->hide();
    if(graphList.size() > 0){
        selectLayer(graphList[0]);
    }
    else{
        ui->displayLayout->addWidget(defaultPage);
        defaultPage->show();
        curGraph = nullptr;
        canvasTitle->setText("START");
        canvasTitle->setMaximumWidth(QFontMetrics(QFont("Corbel Light", 24)).size(Qt::TextSingleLine, "START").width() + 10);
        canvasDesc->setText("Add your first canvas to start");
        curSettingsPage = defaultSettingsPage;
    }
    pageList.erase(pageList.begin() + pageList.indexOf(canvas->settingPage()));
    delete canvas;
    ui->mainWidget->update();
}
*/

/*
void MainWindow::selectSort(MySort *st)
{
    if(!curSort){
        ui->displayLayout->removeWidget(defaultPage);
        defaultPage->hide();
        ui->displayLayout->addWidget(st);
        st->show();
    }else{
        ui->displayLayout->removeWidget(curSort);
        curSort->hide();
        ui->displayLayout->addWidget(st);
        st->show();
    }
    curSort = st;
    st->settingPage()->setParent(ui->mainWidget);
    curSettingsPage = st->settingPage();
    canvasTitle->setText(curSort->name());
    canvasTitle->setMaximumWidth(QFontMetrics(QFont("Corbel Light", 24)).size(Qt::TextSingleLine, canvasTitle->text()).width() + 10);
    canvasDesc->setText(curSort->description());
}
*/

/*
void MainWindow::deleteSort(MySort *st)
{
    int index = sortList.indexOf(st);
    if(index < 0)
        return;
    sortList.erase(sortList.begin() + index);
    ui->displayLayout->removeWidget(curSort);
    curSort->hide();
    if(sortList.size() > 0){
        selectSort(sortList[0]);
    }else{
        ui->displayLayout->addWidget(defaultPage);
        defaultPage->show();
        curSort = nullptr;
        canvasTitle->setText("START");
        canvasTitle->setMaximumWidth(QFontMetrics(QFont("Corbel Light", 24)).size(Qt::TextSingleLine, "START").width() + 10);
        canvasDesc->setText("Add your first canvas to start");
        curSettingsPage = defaultSettingsPage;
    }
    pageList.erase(pageList.begin() + pageList.indexOf(st->settingPage()));
    delete st;
    ui->mainWidget->update();
}
*/

void MainWindow::selectLayer(QWidget *lyr)
{
    qDebug()<<"lyr:"<<lyr;
    if(!curGraph&&!curSort){
        ui->displayLayout->removeWidget(defaultPage);
        defaultPage->hide();
        ui->displayLayout->addWidget(lyr);
        lyr->show();
    }else if(curGraph){
        ui->displayLayout->removeWidget(curGraph);
        curGraph->hide();
    }else if(curSort){
        ui->displayLayout->removeWidget(curSort);
        curSort->hide();
    }

    if(strcmp(lyr->metaObject()->className(),"MyCanvas")==0){
        curGraph = qobject_cast<MyCanvas*>(lyr);
        curSort = nullptr;
        ui->displayLayout->addWidget(curGraph);
        curGraph->settingPage()->setParent(ui->mainWidget);
        curSettingsPage = curGraph->settingPage();
        canvasTitle->setText(curGraph->name());
        canvasTitle->setMaximumWidth(QFontMetrics(QFont("Corbel Light", 24)).size(Qt::TextSingleLine, canvasTitle->text()).width() + 10);
        canvasDesc->setText(curGraph->description());
        curGraph->show();
    }else if(strcmp(lyr->metaObject()->className(),"MySort")==0){
        curSort = qobject_cast<MySort*>(lyr);
        qDebug()<<"curSort"<<curSort;
        curGraph = nullptr;
        ui->displayLayout->addWidget(curSort);
        curSort->settingPage()->setParent(ui->mainWidget);
        curSettingsPage = curSort->settingPage();
        canvasTitle->setText(curSort->name());
        canvasTitle->setMaximumWidth(QFontMetrics(QFont("Corbel Light", 24)).size(Qt::TextSingleLine, canvasTitle->text()).width() + 10);
        canvasDesc->setText(curSort->description());
        curSort->show();
    }
}

void MainWindow::deleteLayer(QWidget *lyr)
{
    if(strcmp(lyr->metaObject()->className(),"MyCanvas")==0){
        MyCanvas *cur = qobject_cast<MyCanvas*>(lyr);
        int index = graphList.indexOf(cur);
        if(index < 0)
            return;
        graphList.erase(graphList.begin() + index);
        ui->displayLayout->removeWidget(curGraph);
        curGraph->hide();
        pageList.erase(pageList.begin() + pageList.indexOf(cur->settingPage()));
        if(graphList.size() > 0){
            selectLayer(graphList[0]);
        }else if(sortList.size() > 0){
            selectLayer(sortList[sortList.size() - 1]);
        }
        delete cur;
    }else if(strcmp(lyr->metaObject()->className(),"MySort")==0){
        MySort *cur = qobject_cast<MySort*>(lyr);
        int index = sortList.indexOf(cur);
        if(index < 0)
            return;
        sortList.erase(sortList.begin() + index);
        ui->displayLayout->removeWidget(curSort);
        curSort->hide();
        pageList.erase(pageList.begin() + pageList.indexOf(cur->settingPage()));
        if(sortList.size() > 0){
            selectLayer(sortList[0]);
        }else if(graphList.size() > 0){
            selectLayer(graphList[graphList.size() - 1]);
        }
        delete cur;
    }

    if(!sortList.size() && !graphList.size()){
        ui->displayLayout->addWidget(defaultPage);
        defaultPage->show();
        curGraph = nullptr;
        canvasTitle->setText("START");
        canvasTitle->setMaximumWidth(QFontMetrics(QFont("Corbel Light", 24)).size(Qt::TextSingleLine, "START").width() + 10);
        canvasDesc->setText("Add your first canvas to start");
        curSettingsPage = defaultSettingsPage;
    }
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
