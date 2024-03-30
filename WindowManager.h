#pragma once

#include <QObject>

#include "TalkWindowShell.h"

class WindowManager  : public QObject
{
	Q_OBJECT

public:
	WindowManager();
	~WindowManager();

public:
	QWidget* findWindowName(const QString& qsWindowName);
	void deleteWindowName(const QString& qsWindowName);
	void addWindowName(const QString& qsWindowName,QWidget* qWidget);

	static WindowManager* getInstance();
	TalkWindowShell* getTalkWindowShell();
	QString getCreatingTalkId();
	void addNewTalkWindow(const QString& uid);//GroupType groupType = FirstClass,const QString& strPeople = "");
	
private:
	TalkWindowShell* m_talkwindowshell;	//多个窗口中的当前窗口
	QMap<QString, QWidget*> m_windowMap;
	QString m_strCreatingTalkId = "";	//正在创建的聊天窗口(QQ号)
};
