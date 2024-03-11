#pragma once

#include <QWidget>
#include "TalkWindowShell.h"
#include "ui_TalkWindow.h"

class TalkWindow : public QWidget
{
	Q_OBJECT

public:
	TalkWindow(QWidget *parent,const QString& uid,GroupType groupType);
	~TalkWindow();

public:
	void addEmotionImage(int emotionNum);
	void setWindowName(const QString& name);
private slots:
	void onSendBtnClicked(bool);
private:
	void initControl();


	void initPtoPTalk();
	void initCompanyTalk();	//初始化各种群聊聊天
	void initPersonelTalk();
	void initMaketTalk();
	void initDevelopTalk();
private:
	Ui::TalkWindowClass ui;
	QString m_talkId;
	GroupType m_groupType;

};