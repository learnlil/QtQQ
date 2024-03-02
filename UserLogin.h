#pragma once

#include "basicwindow.h"
#include "ui_UserLogin.h"

class UserLogin : public BasicWindow
{
	Q_OBJECT

public:
	UserLogin(QWidget *parent = nullptr);
	~UserLogin();

private slots:
	void onLoginButtonClicked();

private:
	void initControl();
private:
	Ui::UserLoginClass ui;
};
