#include "CCMainwindow.h"
#include <QProxyStyle>
#include <QPainter>
#include "SkinWindow.h"
#include <QTimer>
#include "SysTray.h"
#include "NotifyManager.h"
#include <QHBoxLayout>
#include <QEvent>
#include <QTreeWidgetItem>
#include <QMouseEvent>
#include <QApplication>
#include <QSqlQuery>
#include "RootContatItem.h"
#include "ContactItem.h"
#include "WindowManager.h"
#include "TalkWindowShell.h"

QString gstrLoginHeadPath;
extern QString gLoginEmployeeID;

class CustomProxyStyle :public QProxyStyle
{
public:
    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* option,
        QPainter* painter, const QWidget* widget = nullptr) const
    {
        if (element == PE_FrameFocusRect)
        {
            return;
        }
        else
        {
            QProxyStyle::drawPrimitive(element, option, painter, widget);
        }
    }
};

void CCMainwindow::initTimer()
{
    QTimer* timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, [this] {
        static int level = 0;
        if (level == 99)
            level = 0;
        level++;
        setLevelPixmap(level);
        });
    timer->start();
}

QString CCMainwindow::getHeadPicturePath()
{
    QString strPicturePath;

    if (!m_isAccountLogin)   //QQ号登录
    {
        QString strSql(QString("SELECT picture FROM tab_employee WHERE employeeID = %1").arg(gLoginEmployeeID));
        QSqlQuery queryPicture;
        queryPicture.prepare(strSql);
        queryPicture.exec();
        queryPicture.next();
        
        strPicturePath = queryPicture.value(0).toString();
    }
    else                     //账号登录
    {
        QString strSqlProfix(QString("SELECT employeeID FROM tab_accounts WHERE account = %1").arg(gLoginEmployeeID));
        QSqlQuery queryEmployeeID;
        queryEmployeeID.prepare(strSqlProfix);
        queryEmployeeID.exec();
        queryEmployeeID.next();
        int employeeID = queryEmployeeID.value(0).toInt();
        queryEmployeeID.finish();

        QString strSql(QString("SELECT picture FROM tab_employee WHERE employeeID = %1").arg(employeeID));
        QSqlQuery queryPicture;
        queryPicture.prepare(strSql);
        queryPicture.exec();
        queryPicture.next();
        strPicturePath = queryPicture.value(0).toString();
        queryPicture.finish();
    }

    gstrLoginHeadPath = strPicturePath;
    return strPicturePath;
}

void CCMainwindow::updateSearchStyle()
{
    ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{background-color:rgba(%1,%2,%3,50);border-bottom:1px solid rgba(%1,%2,%3,30)}\
                QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/search_icon.png)}")
                .arg(m_colorBackground.red())
                .arg(m_colorBackground.green())
                .arg(m_colorBackground.blue()));
}

void CCMainwindow::initContactTree()
{
    //展开与收缩时的信号
    connect(ui.treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onItemClicked(QTreeWidgetItem*,int)));
    connect(ui.treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(onItemExpanded(QTreeWidgetItem*)));
    connect(ui.treeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(onItemCollapsed(QTreeWidgetItem*)));
    connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*,int)));

    //根节点
    QTreeWidgetItem* pRootGroupItem = new QTreeWidgetItem;
    pRootGroupItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    pRootGroupItem->setData(0, Qt::UserRole, 0);//根项数据设置0
    RootContatItem* pItemName = new RootContatItem(true, ui.treeWidget);

    //获取群号ID
    QString strSqlCode = QString("SELECT departmentID FROM tab_department WHERE departmentName = '%1'").arg(QString::fromUtf8("软工网211群"));
    QSqlQuery queryCompDepID;
    queryCompDepID.prepare(strSqlCode);
    queryCompDepID.exec();
    queryCompDepID.first();
    int CompDecpID = queryCompDepID.value(0).toInt();
    queryCompDepID.finish();
    //获取QQ登录者所在的群ID
    strSqlCode = QString("SELECT departmentID FROM tab_employee WHERE employeeID = %1").arg(gLoginEmployeeID);
    QSqlQuery querySelfDepID;
    querySelfDepID.prepare(strSqlCode);
    querySelfDepID.exec();
    querySelfDepID.first();
    int SelfDecpID = querySelfDepID.value(0).toInt();
    querySelfDepID.finish();


    //初始化公司群以及登陆者所在的群
    addStuClass(pRootGroupItem, CompDecpID);
    addStuClass(pRootGroupItem, SelfDecpID);

    QString strGroupName = QString::fromUtf8("RB软工网");
    pItemName->setText(strGroupName);

    //插入分组节点
    ui.treeWidget->addTopLevelItem(pRootGroupItem);
    ui.treeWidget->setItemWidget(pRootGroupItem, 0, pItemName);
}

void CCMainwindow::addStuClass(QTreeWidgetItem* pRootGroupItem,int DepID)
{
    QTreeWidgetItem* pChild = new QTreeWidgetItem;
    QPixmap pix = QPixmap(":/Resources/MainWindow/head_mask.png");
    //添加子节点,子节点数据设置1
    pChild->setData(0, Qt::UserRole, 1);
    pChild->setData(0, Qt::UserRole + 1, DepID);

    //获取部门头像
    QPixmap groupPix;
    QString strSqlCode = QString("SELECT picture FROM tab_department WHERE departmentID = %1").arg(DepID);
    QSqlQuery queryPicture;
    queryPicture.prepare(strSqlCode);
    queryPicture.exec();
    queryPicture.first();
    groupPix.load(queryPicture.value(0).toString());
    queryPicture.finish();
    //h获取部门名称
    QString strDepName;
    strSqlCode = QString("SELECT departmentName FROM tab_department WHERE departmentID = %1").arg(DepID);
    QSqlQuery queryDepName;
    queryDepName.prepare(strSqlCode);
    queryDepName.exec();
    queryDepName.first();
    strDepName = queryDepName.value(0).toString();
    queryDepName.finish();
    ContactItem* pContactItem = new ContactItem(ui.treeWidget);
    pContactItem->setHeadPixmap(getRoundImage(groupPix,pix,pContactItem->getHeadLabelSize()));
    pContactItem->setUserName(strDepName);

    pRootGroupItem->addChild(pChild);
    ui.treeWidget->setItemWidget(pChild, 0, pContactItem);

 //   m_groupMap.insert(pChild, strDepName);
}

CCMainwindow::CCMainwindow(QString account,bool isAccountLogin,QWidget *parent)
    : BasicWindow(parent)
    ,m_isAccountLogin(isAccountLogin)
    ,m_account(account)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() | Qt::Tool);
    loadStyleSheet("CCMainWindow");
    setHeadPixmap(getHeadPicturePath());
    initControl();
    initTimer();
}

CCMainwindow::~CCMainwindow()
{}

void CCMainwindow::initControl()
{
    //取消树获取焦点时绘制的边框
    ui.treeWidget->setStyle(new CustomProxyStyle);
    setLevelPixmap(12);
    //setHeadPixmap(":/Resources/MainWindow/girl.png");
    setStatusMenuIcon(":/Resources/MainWindow/StatusSucceeded.png");

    QHBoxLayout* appUpLayout = new QHBoxLayout;
    appUpLayout->setContentsMargins(0, 0, 0, 0);
    appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_7.png", "app_7"));
    appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_2.png", "app_2"));
    appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_3.png", "app_3"));
    appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_4.png", "app_4"));
    appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_5.png", "app_5"));
    appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_6.png", "app_6"));
    appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/skin.png", "app_skin"));
    appUpLayout->addStretch();
    appUpLayout->setSpacing(2);
    ui.appWidget->setLayout(appUpLayout);

    ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_10.png", "app_10"));
    ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_11.png", "app_11"));
    ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_8.png", "app_8"));
    ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_9.png", "app_9"));
    ui.bottomLayout_up->addStretch();

    initContactTree();
    
    //个性签名
    ui.lineEdit->installEventFilter(this);
    //好友搜索
    ui.searchLineEdit->installEventFilter(this);

    connect(ui.sysmin, SIGNAL(clicked(bool)), this, SLOT(onShowMin(bool)));
    connect(ui.sysclose, SIGNAL(clicked(bool)), this, SLOT(onShowClose(bool)));
    
    connect(NotifyManager::getInstance(), &NotifyManager::signalSkinChanged, [this]()
        {
            updateSearchStyle();
        });

    SysTray* systray = new SysTray(this);

}

void CCMainwindow::setUserName(const QString& username)
{
    ui.nameLabel->adjustSize();
    //文本过长则进行省略号省略
    QString name = ui.nameLabel->fontMetrics().elidedText(username,Qt::ElideRight,ui.nameLabel->width());
    ui.nameLabel->setText(name);

}

void CCMainwindow::setLevelPixmap(int level)
{
    QPixmap levelPixmap(ui.levelBtn->size());
    levelPixmap.fill(Qt::transparent);

    QPainter painter(&levelPixmap);
    painter.drawPixmap(0, 4, QPixmap(":/Resources/MainWindow/lv.png"));

    int unitNum = level % 10;   //个位
    int tenNum = level / 10;    //十位

    //十位,截取图片中的部分进行绘制
    painter.drawPixmap(10, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"), tenNum * 6, 0, 6, 7);

    painter.drawPixmap(16, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"), unitNum * 6, 0, 6, 7);

    ui.levelBtn->setIcon(levelPixmap);
    ui.levelBtn->setIconSize(ui.levelBtn->size());
}

void CCMainwindow::setHeadPixmap(const QString& headPath)
{
    QPixmap pix;
    pix.load(":/Resources/MainWindow/head_mask.png");
    ui.headLabel->setPixmap(getRoundImage(QPixmap(headPath), pix, ui.headLabel->size()));
}

void CCMainwindow::setStatusMenuIcon(const QString& statusPath)
{
    QPixmap statusBtnPixmap(ui.statusBtn->size());
    statusBtnPixmap.fill(Qt::transparent);

    QPainter painter(&statusBtnPixmap);
    painter.drawPixmap(4, 4, QPixmap(statusPath));

    ui.statusBtn->setIcon(statusBtnPixmap);
    ui.statusBtn->setIconSize(ui.statusBtn->size());

}

QWidget* CCMainwindow::addOtherAppExtension(const QString& appPath, const QString& appName)
{
    QPushButton* btn = new QPushButton(this);
    btn->setFixedSize(20, 20);

    QPixmap pixmap(btn->size());
    pixmap.fill(Qt::transparent);
    
    QPainter painter(&pixmap);
    QPixmap appPixmap(appPath);
    painter.drawPixmap((btn->width() - appPixmap.width()) / 2, (btn->height() - appPixmap.height()) / 2,appPixmap);
    btn->setIcon(pixmap);
    btn->setIconSize(btn->size());
    btn->setObjectName(appName);
    btn->setProperty("hasborder", true);

    connect(btn, &QPushButton::clicked, this, &CCMainwindow::onAppIconClicked);
    return btn;
}

void CCMainwindow::resizeEvent(QResizeEvent* event)
{
    setUserName(QString::fromUtf8("RB软工网212中原工学院"));
    BasicWindow::resizeEvent(event); 
}

bool CCMainwindow::eventFilter(QObject* obj, QEvent* event)
{
    if (ui.searchLineEdit == obj)
    {
        //键盘焦点事件
        if (event->type() == QEvent::FocusIn)
        {
            ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{background-color:rgb(255,255,255);border-bottom:1px solid rgba(%1,%2,%3,100)}\
                QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/main_search_deldown.png)}\
                QPushButton#searchBtn:hover{border-image:url(:/Resources/MainWindow/search/main_search_delhighlight.png)}\
                QPushButton#searchBtn:pressed{border-image:url(:/Resources/MainWindow/search/main_search_delhighdown.png)}")
                  .arg(m_colorBackground.red())
                  .arg(m_colorBackground.green())
                  .arg(m_colorBackground.blue()));

        }
        else if (event->type() == QEvent::FocusOut)
        {
            updateSearchStyle();
        }
    }
    return false;
}
void CCMainwindow::mousePressEvent(QMouseEvent* event)
{
    if (qApp->widgetAt(event->pos())!=ui.searchLineEdit && ui.searchLineEdit->hasFocus())
    {
        ui.searchLineEdit->clearFocus();
    }
    else if (qApp->widgetAt(event->pos()) != ui.lineEdit && ui.lineEdit->hasFocus())
    {
        ui.lineEdit->clearFocus();
    }

    BasicWindow::mousePressEvent(event);
}
void CCMainwindow::onItemClicked(QTreeWidgetItem* item, int column)
{
    bool bIsChild = item->data(0, Qt::UserRole).toBool();
    if (!bIsChild)
    {
        //未展开则展开
        item->setExpanded(!item->isExpanded());
    }

}
void CCMainwindow::onItemExpanded(QTreeWidgetItem* item)
{
    bool bIsChild = item->data(0, Qt::UserRole).toBool();
    if (!bIsChild)
    {
        //dynamic_cast 将基类对象指针(或引用)转换到继承类指针
        RootContatItem* p_rootItem = dynamic_cast<RootContatItem*>(ui.treeWidget->itemWidget(item, 0));
        if (p_rootItem)
        {
            p_rootItem->setExpanding(true);
        }
        
    }
}

void CCMainwindow::onItemCollapsed(QTreeWidgetItem* item)
{
    bool bIdChild = item->data(0, Qt::UserRole).toBool();
    if (!bIdChild)
    {
        RootContatItem* p_rootItem = dynamic_cast<RootContatItem*>(ui.treeWidget->itemWidget(item, 0));
        if (p_rootItem)
        {
            p_rootItem->setExpanding(false);

        }
    }
}

void CCMainwindow::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    bool bIsChild = item->data(0, Qt::UserRole).toBool();
    if (bIsChild)
    {
        WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString());
    }
    //    //QString strGroup = m_groupMap.value(item);

    //    if (strGroup == QString::fromUtf8("RB软工网211"))
    //    {
    //        WindowManager::getInstance()->addNewTalkWindow(item->data(0,Qt::UserRole+1).toString(),COMPANY);

    //    }
    //    else if (strGroup == QString::fromUtf8("RB软工网212"))
    //    {
    //        WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), PERSONELGROUP);
    //    }
    //    else if (strGroup == QString::fromUtf8("RB软工网213"))
    //    { 
    //        WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), MAKETGROUP);
    //    } 
    //    else if (strGroup == QString::fromUtf8("RB软工网214"))
    //    {
    //        WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), DEVELOPMENT);
    //    }
    //}
}

void CCMainwindow::onAppIconClicked()
{
    //判断信号发送者的对象名是否是app_skin
    if (sender()->objectName() == "app_skin")
    {
        SkinWindow* skinWindow = new SkinWindow;
        skinWindow->show();
    }
}
