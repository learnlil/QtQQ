#include "CCMainwindow.h"
#include <QProxyStyle>
#include <QPainter>
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

CCMainwindow::CCMainwindow(QWidget *parent)
    : BasicWindow(parent)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() | Qt::Tool);
    loadStyleSheet("CCMainWindow");
    initControl();
}

CCMainwindow::~CCMainwindow()
{}

void CCMainwindow::initControl()
{
    //取消树获取焦点时绘制的边框
    ui.treeWidget->setStyle(new CustomProxyStyle);
    setLevelPixmap(12);
    setHeadPixmap(":/Resources/MainWindow/girl.png");
    setStatusMenuIcon(":/Resources/MainWindow/StatusSucceeded.png");

    
}

void CCMainwindow::setUserName(const QString& username)
{
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
