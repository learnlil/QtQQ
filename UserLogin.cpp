#include "UserLogin.h"
#include "CCMainwindow.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

QString gLoginEmployeeID;	//登录的QQ号

UserLogin::UserLogin(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initTitleBar();
	setTitleBarTitle("",":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("UserLogin");
	initControl();
}

UserLogin::~UserLogin()
{}

void UserLogin::initControl()
{
	QLabel* headLabel = new QLabel(this);
	headLabel->setFixedSize(68, 68);
	QPixmap pix(":/Resources/MainWindow/head_mask.png");
	headLabel->setPixmap(getRoundImage(QPixmap(":/Resources/MainWindow/girl.png"),pix,headLabel->size()));
	headLabel->move(width() / 2 - 34, ui.titleWidget->height() - 34);
	connect(ui.loginBtn, &QPushButton::clicked, this, &UserLogin::onLoginButtonClicked);
	
	if (!connectMySql())
	{
		QMessageBox::information(NULL,QString::fromUtf8("提示"),
			QString::fromUtf8("连接数据库失败"));
		close();
	}
}

bool UserLogin::connectMySql()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
	db.setHostName("127.0.0.1");
	db.setDatabaseName("qtqq");
	db.setUserName("root");
	db.setPassword("123456");
	if (!db.open())
		return false;
	return true;
}

bool UserLogin::veryfyAccountCode(bool& isAccountLogin, QString& strAccountInput)
{
	QString strAccount = ui.editUserAccount->text();
	QString strPassword = ui.editPassword->text();

	//输入(QQ号登录)
	QString strSqlCode = QString("SELECT code FROM tab_accounts WHERE employeeID = %1").arg(strAccount);
	QSqlQuery queryEmployeeID;
	queryEmployeeID.prepare(strSqlCode);
	queryEmployeeID.exec();
	
	if (queryEmployeeID.first())//指向结果集第一条
	{
		//数据库中QQ号对应的密码 
		QString strCode = queryEmployeeID.value(0).toString();
		
		if (strCode == strPassword)
		{
			gLoginEmployeeID = strAccount;
			isAccountLogin = false;
			strAccountInput = strAccount;
			return true;
		}
		else
		{
			return false;
		}
	}
	queryEmployeeID.finish();

	//if (queryEmployeeID.lastError().type() != QSqlError::NoError)
	//{
	//	qDebug() << "Error: " << queryEmployeeID.lastError().text();
	//}
	//else if (queryEmployeeID.size() == 0)
	//{
	//	qDebug() << "No results found";
	//}

	//账号登录
	strSqlCode = QString("SELECT code,employeeID FROM tab_accounts WHERE account = %1").arg(strAccount);
	QSqlQuery queryAccount;
	queryAccount.prepare(strSqlCode);

	queryAccount.exec();
	
	if (queryAccount.first())
	{
		QString strCode = queryAccount.value(0).toString();
		if (strCode == strPassword)
		{
			gLoginEmployeeID = queryAccount.value(1).toString();
			isAccountLogin = true;
			strAccountInput = strAccount;
			return true;
		}
		else
		{
			return false;
		}
	}
	queryEmployeeID.finish();
	return false;
}

void UserLogin::onLoginButtonClicked()
{
	bool isAccountLogin;
	QString strAccount;
	if (!veryfyAccountCode(isAccountLogin,strAccount))
	{
		QMessageBox::information(NULL, QString::fromUtf8("提示"),
			QString::fromUtf8("您输入的账号或者密码有误,请重新输入"));
		ui.editPassword->setText("");
		ui.editUserAccount->setText("");
		return;
	}

	//更新登录状态为登录
	QString strSqlStatus = QString("UPDATE tab_employee SET online = 2 WHERE employeeID = %1").arg(gLoginEmployeeID);
	QSqlQuery sqlStatus;
	sqlStatus.prepare(strSqlStatus);
	sqlStatus.exec();

	close();
	CCMainwindow* mainwindow = new CCMainwindow(strAccount,isAccountLogin);
	mainwindow->show();
}