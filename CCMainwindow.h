#pragma once

#include <QtWidgets/QWidget>
#include "ui_CCMainwindow.h"

class CCMainwindow : public QWidget
{
    Q_OBJECT

public:
    CCMainwindow(QWidget *parent = nullptr);
    ~CCMainwindow();

private:
    Ui::CCMainwindowClass ui;
};
