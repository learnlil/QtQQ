#pragma once

#include <QDialog>
#include "TitleBar.h"

class BasicWindow  : public QDialog
{
	Q_OBJECT

public:
	BasicWindow(QWidget *parent = nullptr);
	virtual ~BasicWindow();
public:
	void loadStyleSheet(const QString& sheetName);		//加载样式表
	//获取头像(圆)
	QPixmap getRoundImage(const QPixmap& src, QPixmap& mask, QSize masksize = QSize(0, 0));

private:
	void initBackgroundColor();					//初始化背景颜色
protected:
	void paintEvent(QPaintEvent*);				//绘制事件
	void mousePressEvent(QMouseEvent* event);	//鼠标事件
	void mouseMoveEvent(QMouseEvent* event);	//鼠标移动事件
	void mouseReleaseEvent(QMouseEvent* event); //鼠标释放事件

protected:
	void initTitleBar(ButtonType buttontype = MIN_BUTTON);
	void setTitleBarTitle(const QString& title, const QString& icon = "");
public slots:
	void onShowClose(bool);
	void onShowMin(bool);
	void onShowHide(bool);
	void onShowNormal(bool);
	void onShowQuit(bool);
	void onSignalSkinChanged(const QColor &color);

	void onButtonMinClicked();
	void onButtonRestoreClicked();
	void onButtonMaxClicked();
	void onButtonCloseClicked();

protected:
	QPoint m_mousePoint;	//鼠标位置
	bool m_mousePressed;	//鼠标是否按下
	QColor m_colorBackground;//背景色
	QString m_styleName;	//样式文件名称
	TitleBar* _titleBar;	//标题栏
};
