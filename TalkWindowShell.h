#pragma once

#include "ui_TalkWindowShell.h"
#include "basicwindow.h"

#include <QMap>


class TalkWindow;
class TalkWindowItem;
class QListWidgetItem;
class EmotionWindow;

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
	void addTalkWindow(TalkWindow* talkWindow,TalkWindowItem* talkWindowItem,GroupType gouptype);	//添加新的聊天窗口
	void setCurrentWidget(QWidget* widget);										//设置当前聊天窗口
	const QMap<QListWidgetItem*, QWidget*>& getTalkWindowItemMap() const;
private:
	void initControl();//初始化控件

public slots:
	void onEmotionBtnClicked(bool);					//表情按钮点击后执行的槽函数
	
private slots:
	void onTalkWindowItemClicked(QListWidgetItem* item);	//左侧列表点击后执行的槽函数
	void onEmotionItemClicked(int emotionNum);				//表情被选中

private:
	Ui::TalkWindowShellClass ui;

	QMap<QListWidgetItem*, QWidget*> m_talkWindowItemMap;//打开的聊天窗口
	EmotionWindow* m_emotionWindow;						 //表情窗口

};
