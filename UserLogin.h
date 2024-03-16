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
	bool connectMySql();
	bool veryfyAccountCode(bool &isAccountLogin,QString &strAccountInput);
private:
	Ui::UserLogin ui;
};
