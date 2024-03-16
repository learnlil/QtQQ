#pragma once

#include "basicwindow.h"
#include "ui_CCMainwindow.h"

class QTreeWidgetItem;

class CCMainwindow : public BasicWindow
{
    Q_OBJECT

public:
    CCMainwindow(QString account,bool isAccountLogin,QWidget *parent = nullptr);
    ~CCMainwindow();
    void setUserName(const QString& username);
    void setLevelPixmap(int level);
    void setHeadPixmap(const QString& headPath);
    void setStatusMenuIcon(const QString& statusPath);
    //添加应用部件
    QWidget* addOtherAppExtension(const QString& appPath, const QString& appName);

private:
    void initControl();
    void initTimer();
    QString getHeadPicturePath();
    void updateSearchStyle();//更新搜索样式
    void initContactTree();
    void addStuClass(QTreeWidgetItem* pRootGroupItem, int DepID);
private:
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event);
    void mousePressEvent(QMouseEvent* event);

private slots:
    void onItemClicked(QTreeWidgetItem* item, int column);
    void onItemExpanded(QTreeWidgetItem* item);
    void onItemCollapsed(QTreeWidgetItem* item);
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onAppIconClicked();
private:
    Ui::CCMainwindowClass ui;
    bool m_isAccountLogin;
    QString m_account;      //bool true 时是账号,false时是qq号
//    QMap<QTreeWidgetItem*, QString> m_groupMap; //所有分组的分组项
};
