#pragma once

#include <QWidget>
#include "ui_TalkWindowItem.h"

class TalkWindowItem : public QWidget
{
	Q_OBJECT

public:
	TalkWindowItem(QWidget *parent = nullptr);
	~TalkWindowItem();

	void setHeadPixmap(const QString& pixmap);
	void setMsgLabelContent(const QString& msg);
	QString getMsgLabelText();


private:
	void initControl();
signals:
	void signalCloseClicked();
private:
	void enterEvent(QEnterEvent* event);
	void leaveEvent(QEvent* event);
	void resizeEvent(QResizeEvent* event);

private:
	Ui::TalkWindowItemClass ui;
};
