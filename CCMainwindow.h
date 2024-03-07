#pragma once

#include "basicwindow.h"
#include "ui_CCMainwindow.h"

class CCMainwindow : public BasicWindow
{
    Q_OBJECT

public:
    CCMainwindow(QWidget *parent = nullptr);
    ~CCMainwindow();
    void initControl();
    void setUserName(const QString& username);
    void setLevelPixmap(int level);
    void setHeadPixmap(const QString& headPath);
    void setStatusMenuIcon(const QString& statusPath);
private:
    Ui::CCMainwindowClass ui;
};
