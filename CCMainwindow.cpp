#include "CCMainwindow.h"
#include <QProxyStyle>

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

}

void CCMainwindow::setUserName(const QString& username)
{
}

void CCMainwindow::setLevelPixmap(int level)
{

}

void CCMainwindow::setHeadPixmap(const QString& headPath)
{
}
