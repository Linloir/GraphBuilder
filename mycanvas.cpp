#include "mycanvas.h"

MyCanvas::MyCanvas(int radius, QString name, QString desc, int structure, int _type, QWidget *parent) :
    QWidget(parent),
    canvasName(name),
    canvasDescription(desc),
    structure_type(structure),
    type(_type)
{
    /* create canvas */
    mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(mainLayout);
    view = new MyGraphicsView(type == UDG ? MyGraphicsView::UDG : MyGraphicsView::DG);
    view->setSceneRect(view->rect());
    view->setStyleSheet("background-color: #FFFFFF;border:1px solid #cfcfcf;border-radius:10px;");
    mainLayout->addWidget(view);
    g = structure == AL ? (AbstractGraph*)(new ALGraph(type)) : (AbstractGraph*)(new AMLGraph(type));
    connect(view, SIGNAL(vexAdded(MyGraphicsVexItem*)), this, SLOT(addVex(MyGraphicsVexItem*)));
    connect(view, SIGNAL(arcAdded(MyGraphicsLineItem*)), this, SLOT(addArc(MyGraphicsLineItem*)));
    connect(view, &MyGraphicsView::visitClear, this, [=](){g->ClearVisit();});
    this->setFocusPolicy(Qt::ClickFocus);

    CreateSettings(radius);
}

MyCanvas::MyCanvas(QTextStream &ts, int radius, QWidget *parent) :
    QWidget(parent)
{
    canvasName = ts.readLine();
    canvasDescription = ts.readLine();
    ts >> structure_type >> type;

    /* create canvas */
    mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(mainLayout);
    view = new MyGraphicsView(type == UDG ? MyGraphicsView::UDG : MyGraphicsView::DG);
    view->setSceneRect(view->rect());
    view->setStyleSheet("background-color: #FFFFFF;border:1px solid #cfcfcf;border-radius:10px;");
    mainLayout->addWidget(view);
    g = structure_type == AL ? (AbstractGraph*)(new ALGraph(type)) : (AbstractGraph*)(new AMLGraph(type));
    connect(view, SIGNAL(vexAdded(MyGraphicsVexItem*)), this, SLOT(addVex(MyGraphicsVexItem*)));
    connect(view, SIGNAL(arcAdded(MyGraphicsLineItem*)), this, SLOT(addArc(MyGraphicsLineItem*)));
    connect(view, &MyGraphicsView::visitClear, this, [=](){g->ClearVisit();});
    view->ReadFromFile(ts);
    for(int i = 0; i < view->arcNum; i++){
        int w;
        ts >> w;
        if(w != 0)
            g->SetWeight(view->lines[i], w);
    }
    this->setFocusPolicy(Qt::ClickFocus);

    CreateSettings(radius);
}

void MyCanvas::CreateSettings(int radius){
    /* create settings page */
    settings = new SlidePage(radius, "SETTINGS", this->parentWidget());
    singleSelectGroup *structureSetting = new singleSelectGroup("Structure", this);
    selectionItem *setAL = new selectionItem("AL", "Adjacent list structure", this);
    selectionItem *setAML = new selectionItem("AML", "Adjacent multiple list", this);
    structureSetting->AddItem(setAL);
    structureSetting->AddItem(setAML);
    structureSetting->SetSelection(structure_type == AL ? setAL : setAML);
    connect(structureSetting, &singleSelectGroup::selectedItemChange, this, [=](int id){
        if(id == 1){
            ALGraph *old = (ALGraph*)g;
            g = old->ConvertToAML();
            old->~ALGraph();
            structure_type = AML;
        }
        else{
            AMLGraph *old = (AMLGraph*)g;
            g = old->ConvertToAL();
            old->~AMLGraph();
            structure_type = AL;
        }
    });
    singleSelectGroup *dirSetting = new singleSelectGroup("Mode", this);
    selectionItem *setDG = new selectionItem("DG", "Directed graph", this);
    selectionItem *setUDG = new selectionItem("UDG", "Undirected graph", this);
    dirSetting->AddItem(setDG);
    dirSetting->AddItem(setUDG);
    dirSetting->SetSelection(type == DG ? setDG : setUDG);
    connect(dirSetting, &singleSelectGroup::selectedItemChange, this, [=](int id){
        g->ConvertType(id == 0 ? AbstractGraph::DG : AbstractGraph::UDG);
        view->setType(id == 0 ? MyGraphicsView::DG : MyGraphicsView::UDG);
        type = id == 0 ? DG : UDG;
    });
    singleSelectGroup *dfsSetting = new singleSelectGroup("Traverse Mode", this);
    selectionItem *setGenerateTree = new selectionItem("Tree", "Generate tree", this);
    selectionItem *setGenerateForest = new selectionItem("Forest", "Generate forest", this);
    dfsSetting->AddItem(setGenerateTree);
    dfsSetting->AddItem(setGenerateForest);
    connect(dfsSetting, &singleSelectGroup::selectedItemChange, this, [=](int id){
        generateForest = id == 1;
    });
    QWidget *whiteSpace = new QWidget(this);
    whiteSpace->setFixedHeight(30);
    horizontalValueAdjuster *aniSpeed = new horizontalValueAdjuster("Animation speed", 0.1, 20, 0.1, this);
    aniSpeed->setValue(1.0);
    connect(aniSpeed, &horizontalValueAdjuster::valueChanged, view, [=](qreal value){view->setAniRate(value);});
    textInputItem *rename = new textInputItem("Name", this);
    rename->setValue(canvasName);
    connect(rename, &textInputItem::textEdited, this, [=](QString text){canvasName = text; emit nameChanged(text);});
    textInputItem *redesc = new textInputItem("Detail", this);
    redesc->setValue(canvasDescription);
    connect(redesc, &textInputItem::textEdited, this, [=](QString text){canvasDescription = text; emit descChanged(text);});
    textButton *hideBtn = new textButton("Hide Unvisited Items", this);
    connect(hideBtn, &textButton::clicked, this, [=](){view->HideUnvisited();});
    textButton *showBtn = new textButton("Show Unvisited Items", this);
    connect(showBtn, &textButton::clicked, this, [=](){view->ShowUnvisited();});
    QWidget *whiteSpace2 = new QWidget(this);
    whiteSpace2->setFixedHeight(30);
    textButton *saveBtn = new textButton("Save to file", this);
    connect(saveBtn, &textButton::clicked, this, [=](){
        QString savePath = QFileDialog::getSaveFileName(this, tr("Save map"), " ", tr("Map file(*.map)"));
        if(!savePath.isEmpty())
            SaveToFile(savePath);
    });
    textButton *delBtn = new textButton("Delete", "#0acb1b45","#1acb1b45","#2acb1b45",this);
    connect(delBtn, &textButton::clicked, this, [=](){emit setDel(this);});
    settings->AddContent(delBtn);
    settings->AddContent(saveBtn);
    settings->AddContent(whiteSpace2);
    settings->AddContent(showBtn);
    settings->AddContent(hideBtn);
    settings->AddContent(dfsSetting);
    settings->AddContent(dirSetting);
    settings->AddContent(structureSetting);
    settings->AddContent(aniSpeed);
    settings->AddContent(whiteSpace);
    settings->AddContent(redesc);
    settings->AddContent(rename);
    settings->show();

    QTimer *delay = new QTimer(this);
    connect(delay, &QTimer::timeout, this, [=](){Init();});
    delay->setSingleShot(true);
    delay->start(10);
}

void MyCanvas::Init(){
    /* Create info widget */
    infoWidget = new QWidget(this);
    mainLayout->addWidget(infoWidget);
    mainLayout->setStretch(0, 7);
    mainLayout->setStretch(1, 3);
    infoWidget->setMinimumWidth(250);
    infoWidget->setMaximumWidth(500);

    //Set basic layout
    QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);
    infoWidget->setLayout(infoLayout);
    infoLayout->setContentsMargins(10, 0, 0, 0);
    infoLayout->setAlignment(Qt::AlignTop);

    QFont titleFont = QFont("Corbel", 20);

    QWidget *upper = new QWidget(infoWidget);
    QVBoxLayout *upperLayout = new QVBoxLayout(upper);
    upper->setLayout(upperLayout);
    upperLayout->setContentsMargins(0, 0, 0, 0);
    upper->setContentsMargins(0, 0, 0, 0);
    pageName = new QLabel(infoWidget);
    pageName->setText("INFO");
    pageName->setFont(titleFont);
    pageName->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    pageName->setStyleSheet("color:#2c2c2c");
    QWidget *upperSplitter = new QWidget(upper);
    upperSplitter->setFixedSize(30, 6);
    upperSplitter->setStyleSheet("background-color:#3c3c3c;border-radius:3px;");
    upperLayout->addWidget(pageName);
    upperLayout->addWidget(upperSplitter);

    QWidget *lower = new QWidget(infoWidget);
    QVBoxLayout *lowerLayout = new QVBoxLayout(lower);
    lower->setLayout(lowerLayout);
    lowerLayout->setContentsMargins(0, 0, 0, 0);
    QLabel *logLabel = new QLabel(lower);
    logLabel->setText("LOG");
    logLabel->setFont(titleFont);
    logLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    logLabel->setStyleSheet("color:#2c2c2c");
    QWidget *lowerSplitter = new QWidget(lower);
    lowerSplitter->setFixedSize(30, 6);
    lowerSplitter->setStyleSheet("background-color:#3c3c3c;border-radius:3px;");
    ScrollAreaCustom *logDisplay = new ScrollAreaCustom(lower);
    logDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lowerLayout->addWidget(logLabel);
    lowerLayout->addWidget(lowerSplitter);
    lowerLayout->addWidget(logDisplay);

    infoLayout->addWidget(upper);
    infoLayout->addWidget(lower);

    //Add specific items and connections
    //Default page
    QWidget *defInfoPage = new QWidget(infoWidget);
    QVBoxLayout *defInfoLayout = new QVBoxLayout(defInfoPage);
    defInfoPage->setLayout(defInfoLayout);
    defInfoLayout->setContentsMargins(0, 0, 0, 0);
    defInfoLayout->setAlignment(Qt::AlignTop);
    QWidget *defTextItems = new QWidget(defInfoPage);
    defTextItems->setObjectName("DefTextItems");
    defTextItems->setStyleSheet("QWidget#DefTextItems{border:1px solid #cfcfcf;border-radius:5px;}");
    QVBoxLayout *defTextLayout = new QVBoxLayout(defTextItems);
    defTextItems->setLayout(defTextLayout);
    defTextLayout->setContentsMargins(0, 5, 0, 5);
    textInputItem *textName = new textInputItem("Name", defInfoPage);
    textName->setValue(canvasName);
    connect(this, &MyCanvas::nameChanged, this, [=](){textName->setValue(canvasName);});
    textName->setEnabled(false);
    defTextLayout->addWidget(textName);
    textInputItem *textDesc = new textInputItem("Detail", defInfoPage);
    textDesc->setValue(canvasDescription);
    connect(this, &MyCanvas::descChanged, this, [=](){textDesc->setValue(canvasDescription);});
    textDesc->setEnabled(false);
    defTextLayout->addWidget(textDesc);
    textInputItem *vexNumText = new textInputItem("Vex", defInfoPage);
    vexNumText->setValue(QString::asprintf("%d", view->vexNum));
    vexNumText->setEnabled(false);
    defTextLayout->addWidget(vexNumText);
    textInputItem *arcNumText = new textInputItem("Arc", defInfoPage);
    arcNumText->setValue(QString::asprintf("%d", view->arcNum));
    arcNumText->setEnabled(false);
    defTextLayout->addWidget(arcNumText);
    defInfoLayout->addWidget(defTextItems);
    upperLayout->addWidget(defInfoPage);
    defInfoPage->show();

    //VexPage
    QWidget *vexInfoPage = new QWidget(infoWidget);
    QVBoxLayout *vexInfoLayout = new QVBoxLayout(vexInfoPage);
    vexInfoLayout->setContentsMargins(0, 0, 0, 0);
    vexInfoLayout->setAlignment(Qt::AlignTop);
    vexInfoPage->setLayout(vexInfoLayout);
    QWidget *vexTextItems = new QWidget(vexInfoPage);
    vexTextItems->setObjectName("VexTextItems");
    vexTextItems->setStyleSheet("QWidget#VexTextItems{border:1px solid #cfcfcf;border-radius:5px;}");
    QVBoxLayout *vexTextLayout = new QVBoxLayout(vexTextItems);
    vexTextItems->setLayout(vexTextLayout);
    vexTextLayout->setContentsMargins(0, 5, 0, 5);
    textInputItem *textTag = new textInputItem("Tag", vexInfoPage);
    vexTextLayout->addWidget(textTag);
    textInputItem *dijStart = new textInputItem("Start", vexInfoPage);
    dijStart->setValue("Run dijkstra first");
    dijStart->setEnabled(false);
    vexTextLayout->addWidget(dijStart);
    textInputItem *dijDistance = new textInputItem("Dist", vexInfoPage);
    dijDistance->setValue("Infinite");
    dijDistance->setEnabled(false);
    vexTextLayout->addWidget(dijDistance);
    textInputItem *dijPrev = new textInputItem("Prev", vexInfoPage);
    dijPrev->setValue("Run dijkstra first");
    dijPrev->setEnabled(false);
    vexTextLayout->addWidget(dijPrev);
    vexInfoLayout->addWidget(vexTextItems);
    QWidget *traverseBar = new QWidget(vexInfoPage);
    QHBoxLayout *traverseLayout = new QHBoxLayout(traverseBar);
    traverseBar->setLayout(traverseLayout);
    traverseLayout->setContentsMargins(0, 0, 0, 0);
    textButton *startBfs = new textButton("BFS", vexInfoPage);
    traverseLayout->addWidget(startBfs);
    textButton *startDfs = new textButton("DFS", vexInfoPage);
    traverseLayout->addWidget(startDfs);
    vexInfoLayout->addWidget(traverseBar);
    textButton *startDij = new textButton("Start Dijkstra", vexInfoPage);
    startDij->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    vexInfoLayout->addWidget(startDij);
    textButton *delVex = new textButton("Delete", "#1acb1b45","#2acb1b45","#3acb1b45", vexInfoPage);
    vexInfoLayout->addWidget(delVex);
    upperLayout->addWidget(vexInfoPage);
    vexInfoPage->hide();

    //ArcPage
    QWidget *arcInfoPage = new QWidget(infoWidget);
    QVBoxLayout *arcInfoLayout = new QVBoxLayout(arcInfoPage);
    arcInfoLayout->setContentsMargins(0, 0, 0, 0);
    arcInfoLayout->setAlignment(Qt::AlignTop);
    arcInfoPage->setLayout(arcInfoLayout);
    QWidget *arcTextItems = new QWidget(arcInfoPage);
    arcTextItems->setObjectName("VexTextItems");
    arcTextItems->setStyleSheet("QWidget#VexTextItems{border:1px solid #cfcfcf;border-radius:5px;}");
    QVBoxLayout *arcTextLayout = new QVBoxLayout(arcTextItems);
    arcTextItems->setLayout(arcTextLayout);
    arcTextLayout->setContentsMargins(0, 5, 0, 5);
    textInputItem *arcWeight = new textInputItem("Pow", arcInfoPage);
    arcTextLayout->addWidget(arcWeight);
    QRegularExpression re("^[1-9]\\d*$");
    arcWeight->setValidator(new QRegularExpressionValidator(re));
    textInputItem *arcStart = new textInputItem("Start", arcInfoPage);
    arcStart->setValue("NA");
    arcStart->setEnabled(false);
    arcTextLayout->addWidget(arcStart);
    textInputItem *arcEnd = new textInputItem("End", arcInfoPage);
    arcEnd->setValue("NA");
    arcEnd->setEnabled(false);
    arcTextLayout->addWidget(arcEnd);
    arcInfoLayout->addWidget(arcTextItems);
    textButton *reverseBtn = new textButton("Reverse", arcInfoPage);
    arcInfoLayout->addWidget(reverseBtn);
    textButton *delArc = new textButton("Delete", "#1acb1b45","#2acb1b45","#3acb1b45", arcInfoPage);
    arcInfoLayout->addWidget(delArc);
    upperLayout->addWidget(arcInfoPage);
    arcInfoPage->hide();

    connect(view, &MyGraphicsView::vexAdded, this, [=](){vexNumText->setValue(QString::asprintf("%d",view->vexNum));});
    connect(view, &MyGraphicsView::arcAdded, this, [=](){arcNumText->setValue(QString::asprintf("%d",view->arcNum));});
    connect(view, &MyGraphicsView::selected, this, [=](QGraphicsItem *item){
        int type = item->type();
        if(type == MyGraphicsVexItem::Type){
            defInfoPage->hide();
            arcInfoPage->hide();
            vexInfoPage->show();
            textTag->setValue(view->selectedVex()->Text());
            if(g->GetInfoOf(view->selectedVex())->strtVexInfo == nullptr){
                dijStart->setValue("Run dijkstra first");
                dijPrev->setValue("Run dijkstra first");
                dijDistance->setValue("Infinite");
            }
            else{
                dijStart->setValue(g->GetInfoOf(view->selectedVex())->strtVexInfo->gVex->Text());
                if(g->GetInfoOf(view->selectedVex())->preVexID == -1)
                    dijPrev->setValue("This vex");
                else
                    dijPrev->setValue(g->GetInfoOf(g->GetInfoOf(view->selectedVex())->preVexID)->gVex->Text());
                dijDistance->setValue(g->GetInfoOf(view->selectedVex())->distance == 2147483647 ? "Infinite" : QString::asprintf("%d", g->GetInfoOf(view->selectedVex())->distance));
            }
        }
        else if(type == MyGraphicsLineItem::Type){
            defInfoPage->hide();
            vexInfoPage->hide();
            arcInfoPage->show();
            arcWeight->setValue(view->selectedArc()->weightText() == "" ? "1" : view->selectedArc()->weightText());
            arcStart->setValue(view->selectedArc()->stVex()->Text());
            arcEnd->setValue(view->selectedArc()->edVex()->Text());
        }
        else{
            vexInfoPage->hide();
            arcInfoPage->hide();
            defInfoPage->show();
            vexNumText->setValue(QString::asprintf("%d",view->vexNum));
            arcNumText->setValue(QString::asprintf("%d",view->arcNum));
        }
    });
    connect(textTag, &textInputItem::textEdited, this, [=](QString text){
        logDisplay->addWidget(new viewLog("[Vex] | Rename \""+view->selectedVex()->Text()+"\" to \""+text+"\""));
        if(view->selectedVex() != nullptr)
            view->selectedVex()->setText(text);
        if(g->GetInfoOf(view->selectedVex())->strtVexInfo != nullptr){
            dijStart->setValue(g->GetInfoOf(view->selectedVex())->strtVexInfo->gVex->Text());
            if(g->GetInfoOf(view->selectedVex())->preVexID != -1)
                dijPrev->setValue(g->GetInfoOf(g->GetInfoOf(view->selectedVex())->preVexID)->gVex->Text());
        }
    });
    connect(arcWeight, &textInputItem::textEdited, this, [=](QString text){
        logDisplay->addWidget(new viewLog("[Arc] | \""+view->selectedArc()->stVex()->Text()+"\" -> \""+view->selectedArc()->edVex()->Text()+"\" set to "+text));
        g->SetWeight(view->selectedArc(), text.toDouble());
    });
    connect(view, &MyGraphicsView::unselected, this, [=](){
        vexInfoPage->hide();
        arcInfoPage->hide();
        defInfoPage->show();
        vexNumText->setValue(QString::asprintf("%d",view->vexNum));
        arcNumText->setValue(QString::asprintf("%d",view->arcNum));
    });
    connect(startBfs, &textButton::clicked, this, [=](){
        viewLog *newLog = new viewLog("[BFS] | --- BFS start ---");
        newLog->setStyleSheet("color:#0078d4");
        logDisplay->addWidget(newLog);
        g->BFS(view->selectedVex(), generateForest);
        view->hasVisitedItem = true;
    });
    connect(startDfs, &textButton::clicked, this, [=](){
        viewLog *newLog = new viewLog("[DFS] | --- DFS start ---");
        newLog->setStyleSheet("color:#0078d4");
        logDisplay->addWidget(newLog);
        g->DFS(view->selectedVex(), generateForest);
        view->hasVisitedItem = true;
    });
    connect(startDij, &textButton::clicked, this, [=](){
        viewLog *newLog = new viewLog("[Dij] | --- Dijkstra start ---");
        newLog->setStyleSheet("color:#0078d4");
        logDisplay->addWidget(newLog);
        g->Dijkstra(view->selectedVex());
        view->hasVisitedItem = true;
        if(g->GetInfoOf(view->selectedVex())->strtVexInfo == nullptr){
            dijStart->setValue("Run dijkstra first");
            dijPrev->setValue("Run dijkstra first");
            dijDistance->setValue("Infinite");
        }
        else{
            dijStart->setValue(g->GetInfoOf(view->selectedVex())->strtVexInfo->gVex->Text());
            if(g->GetInfoOf(view->selectedVex())->preVexID == -1)
                dijPrev->setValue("This vex");
            else
                dijPrev->setValue(g->GetInfoOf(g->GetInfoOf(view->selectedVex())->preVexID)->gVex->Text());
            dijDistance->setValue(g->GetInfoOf(view->selectedVex())->distance == 2147483647 ? "Infinite" : QString::asprintf("%d", g->GetInfoOf(view->selectedVex())->distance));
        }
    });
    connect(reverseBtn, &textButton::clicked, this, [=](){
        if(g->Type() == AbstractGraph::UDG)
            view->selectedArc()->reverseDirection();
        else{
            g->DelArc(view->selectedArc());
            view->selectedArc()->reverseDirection();
            g->AddArc(view->selectedArc());
        }
    });
    connect(delVex, &textButton::clicked, this, [=](){
        logDisplay->addWidget(new viewLog("[Vex] | Delete vex \""+view->selectedVex()->Text()+"\""));
        g->DelVex(view->selectedVex());
        view->selectedVex()->remove();
        g->ResetDistance();
        g->ClearVisit();
        view->unSelect();
    });
    connect(delArc, &textButton::clicked, this, [=](){
        logDisplay->addWidget(new viewLog("[Arc] | Delete arc \""+view->selectedArc()->stVex()->Text()+"\" -> \""+view->selectedArc()->edVex()->Text()+"\""));
        g->DelArc(view->selectedArc());
        view->selectedArc()->remove();
        g->ResetDistance();
        g->ClearVisit();
        view->unSelect();
    });

    connect(view, &MyGraphicsView::logAdded, this, [=](viewLog* log){logDisplay->addWidget(log);});

}

void MyCanvas::SaveToFile(const QString &path){
    QFile output(path);
    output.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream ts(&output);
    ts << "VFdGeWFXUnZaekl3TURJd05ESTE=\n";
    ts << canvasName << "\n";
    ts << canvasDescription << "\n";
    ts << structure_type << " " << type << "\n";
    view->SaveToFile(ts);
    output.close();
}

void MyCanvas::addVex(MyGraphicsVexItem *vex){
    g->AddVex(vex);
}

void MyCanvas::addArc(MyGraphicsLineItem *arc){
    g->AddArc(arc);
}
