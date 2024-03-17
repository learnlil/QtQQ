#pragma once

#include "ui_TalkWindowShell.h"
#include "basicwindow.h"

#include <QMap>
#include<QTcpSocket>
#include <QUdpSocket>

class TalkWindow;
class TalkWindowItem;
class QListWidgetItem;
class EmotionWindow;

const int gTcpPort = 8888;

enum GroupType
{
	COMPANY = 0,	//总群
	PERSONELGROUP,	//人事
	DEVELOPMENT,	//研发
	MAKETGROUP,		//市场
	PTOP			//单聊
};

class TalkWindowShell : public BasicWindow
{
	Q_OBJECT

public:
	TalkWindowShell(QWidget *parent = nullptr);
	~TalkWindowShell();

public:
	void addTalkWindow(TalkWindow* talkWindow, TalkWindowItem* talkWindowItem,const QString& uid);//GroupType gouptype);	//添加新的聊天窗口
	void setCurrentWidget(QWidget* widget);										//设置当前聊天窗口
	const QMap<QListWidgetItem*, QWidget*>& getTalkWindowItemMap() const;
private:
	void initControl();//初始化控件
	void initTcpSocket();	//初始化TCP
	void initUdpSocket();	//初始化UDP
	void getEmployeesID(QStringList& employeesList);	//获取所有员工QQ号
	bool createJSFile(QStringList& employeesList);
	void handleReceiveMsg(int senderEmployeeID,int msgType,QString strMsg);

public slots:
	void onEmotionBtnClicked(bool);					//表情按钮点击后执行的槽函数

	//客户端发送Tcp数据(数据,数据类型,文件)
	void updateSendTcpMsg(QString& strData, int &msgType,QString fileName = "");
private slots:
	void onTalkWindowItemClicked(QListWidgetItem* item);	//左侧列表点击后执行的槽函数
	void onEmotionItemClicked(int emotionNum);				//表情被选中
	void processPendingData();								//处理广播收到的数据
private:
	Ui::TalkWindowShellClass ui;

	QMap<QListWidgetItem*, QWidget*> m_talkWindowItemMap;//打开的聊天窗口
	EmotionWindow* m_emotionWindow;						 //表情窗口

private:
	QTcpSocket* m_tcpClientSocket;	//Tcp客户端
	QUdpSocket* m_udpReceiver;      //udp接收端
	
};
