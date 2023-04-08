#include "mysort.h"

MySort::MySort(int radius, QString name, QString desc, int _type, QWidget *parent) :
    QWidget(parent),
    sortName(name),
    sortDescription(desc),
    type(_type)
{
    mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(mainLayout);
    sortCanvas = new MainCanvas();
    sortCanvas->setStyleSheet("background-color: #FFFFFF;border:1px solid #cfcfcf;border-radius:10px;");

    mainLayout->addWidget(sortCanvas);

    this->setFocusPolicy(Qt::ClickFocus);

    CreateSettings(radius);
}

MySort::MySort(QTextStream &ts, int radius, QWidget *parent) :
    QWidget(parent)
{
    sortName = ts.readLine();
    sortDescription = ts.readLine();
    ts >> type;

    mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(mainLayout);
    sortCanvas = new MainCanvas();
    mainLayout->addWidget(sortCanvas);

    this->setFocusPolicy(Qt::ClickFocus);

    CreateSettings(radius);
}

void MySort::CreateSettings(int r)
{
    //侧页-单击排序页面设置
    settings = new SlidePage(r, "SETTINGS", this->parentWidget());

    textInputItem * rename = new textInputItem("Name", this);
    rename->setValue(sortName);
    connect(rename, &textInputItem::textEdited, this, [=](QString text){
        sortName = text;
        emit nameChanged(text);
    });

    textInputItem * redesc = new textInputItem("Detail", this);
    redesc->setValue(sortDescription);
    connect(redesc, &textInputItem::textEdited, this, [=](QString text){
        sortDescription = text;
        emit descChanged(text);
    });

    QWidget * whiteSpace = new QWidget(this);
    whiteSpace->setFixedHeight(30);

//    textButton * saveBtn = new textButton("Save to file", this);
//    connect(saveBtn, &textButton::clicked, this, [=](){
//        QString savePath = QFileDialog::getSaveFileName(this, tr("Save Visualizetion"), " ", tr("Map file(*.map)"));
//        if(!savePath.isEmpty()) SaveToFile
//    })

    textButton * delBtn = new textButton("Delete", "#0acb1b45","#1acb1b45","#2acb1b45", this);
    connect(delBtn, &textButton::clicked, this, [=](){emit setDel(this);});
    settings->AddContent(delBtn);
    settings->AddContent(whiteSpace);
    settings->AddContent(redesc);
    settings->AddContent(rename);
    settings->show();

    QTimer *delay = new QTimer(this);
    connect(delay, &QTimer::timeout, this, [=](){Init();});
    delay->setSingleShot(true);
    delay->start(10);
}

void MySort::Init()
{
    sideWidget = new QWidget(this);
    mainLayout->addWidget(sideWidget);
    mainLayout->setStretch(0, 1);
    sideWidget->setFixedWidth(250);

    QVBoxLayout *sideLayout = new QVBoxLayout(sideWidget);
    sideWidget->setLayout((sideLayout));
    sideLayout->setContentsMargins(10, 0, 0, 0);
    sideLayout->setAlignment(Qt::AlignTop);

    contentContainer * info = new contentContainer("INFO", sideWidget);
    contentContainer * control = new contentContainer("CONTROL", sideWidget);
    QWidget *whiteSpace = new QWidget(this);
    sideLayout->addWidget(info);
    sideLayout->addWidget(control);
    sideLayout->addWidget(whiteSpace);
    sideLayout->setStretch(2,1);


    QWidget *defInfoPage = new QWidget(sideWidget);
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
    textInputItem *textName = new textInputItem("Type", defInfoPage);
    textName->setValue(typeName[type]);
    textName->setEnabled(false);
    defTextLayout->addWidget(textName);
    info->AddContent(defTextItems);
    defInfoPage->show();


    textInputItem *num = new textInputItem("Amount", control);
    horizontalValueAdjuster *aniSpeed = new horizontalValueAdjuster("Animation speed", 1, 199, 1, control);
    textButton *strtBtn = new textButton("Start", sideWidget);
    textButton *stopBtn = new textButton("End", "#0acb1b45","#1acb1b45","#2acb1b45", sideWidget);


//    whiteSpace->setSizePolicy(QSizePolicy::Expanding);

    control->AddContent(num);
    control->AddContent(aniSpeed);
    control->AddContent(strtBtn);
    control->AddContent(stopBtn);


    connect(strtBtn, &textButton::clicked, this, [=](){
        if(type != sortCanvas->getSortType()){
            SortObject *obj = SortFactory::getInstance()->createSortObject(type, sortCanvas);
            sortCanvas->setSortObject(type, obj);
        }
        sortCanvas->sort([num](){
            QString v = num->value();
            int d = v.toInt();
            if(d == 0)
                d = 50;
            else if (d > 80)
                d = 80;
            return d;
        }(), aniSpeed->value() + 9);
    });
    connect(stopBtn, &textButton::clicked, this, [=](){
        sortCanvas->stop();
    });
    connect(sortCanvas, &MainCanvas::runFlagChanged, this, [=](bool running){
        num->setEnabled(!running);
        aniSpeed->setEnabled(!running);
        strtBtn->setEnabled(!running);
    });
}





