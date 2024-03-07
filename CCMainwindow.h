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
    
private:
    Ui::CCMainwindowClass ui;
};
