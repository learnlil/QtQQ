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
	void addNewTalkWindow(const QString& uid);//GroupType groupType = COMPANY,const QString& strPeople = "");
	
private:
	TalkWindowShell* m_talkwindowshell;
	QMap<QString, QWidget*> m_windowMap;

};
