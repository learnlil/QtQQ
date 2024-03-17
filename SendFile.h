#pragma once

#include "basicwindow.h"
#include "ui_SendFile.h"

class SendFile : public BasicWindow
{
	Q_OBJECT

public:
	SendFile(QWidget *parent = nullptr);
	~SendFile();

signals://发送文件发射的信号
	void sendFileClicked(QString& strData, int& msgType, QString fileName);

private slots:
	void on_openBtn_clicked();		//打开按钮点击
	void on_sendBtn_clicked();		//发送按钮点击
private:
	Ui::SendFileClass ui;
	QString m_filePath;				//选择的文件路径
};
