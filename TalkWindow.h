#pragma once

#include <QWidget>
#include "TalkWindowShell.h"
#include "ui_TalkWindow.h"

class TalkWindow : public QWidget
{
	Q_OBJECT

public:
	TalkWindow(QWidget* parent, const QString& uid);//,GroupType groupType);
	~TalkWindow();

public:
	void addEmotionImage(int emotionNum);
	void setWindowName(const QString& name);
	QString getTalkId();
private slots:
	void onFileOpenBtnClicked(bool);
	void onSendBtnClicked(bool);
	void onItemDoubleClicked(QTreeWidgetItem* item,int column);
private:
	void initControl();
	void initGroupTalkStatus();
	int getCompDepID();

	void initPtoPTalk();
	void initTalkWindow();
	//void initCompanyTalk();	//初始化各种群聊聊天
	//void initPersonelTalk();
	//void initMaketTalk();
	//void initDevelopTalk();
	void addPeopInfo(QTreeWidgetItem* pRootGoupItem,int employeeID);
private:
	Ui::TalkWindowClass ui;
	QString m_talkId;
	bool m_isGroupTalk;	//是否为群聊
	//GroupType m_groupType;
	QMap<QTreeWidgetItem*, QString> m_groupPeopleMap;//所有分组联系人姓名
	friend class TalkWindowShell;
};
