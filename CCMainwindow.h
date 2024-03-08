#pragma once

#include "basicwindow.h"
#include "ui_CCMainwindow.h"

class CCMainwindow : public BasicWindow
{
    Q_OBJECT

public:
    void initTimer();
    CCMainwindow(QWidget *parent = nullptr);
    ~CCMainwindow();
    void initControl();
    void setUserName(const QString& username);
    void setLevelPixmap(int level);
    void setHeadPixmap(const QString& headPath);
    void setStatusMenuIcon(const QString& statusPath);
    //添加应用部件
    QWidget* addOtherAppExtension(const QString& appPath, const QString& appName);
private slots:
    void onAppIconClicked();
private:
    Ui::CCMainwindowClass ui;
};
