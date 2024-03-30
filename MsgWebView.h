#pragma once

#include <QWebEngineView>//QWebEngineView视图用于显示网页内容
#include <QDomNode>

class MsgHtmlObj : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString msgLHtmlTmpl MEMBER m_msgLHtmlTmpl NOTIFY signalMsgHtml)	
	Q_PROPERTY(QString msgRHtmlTmpl MEMBER m_msgRHtmlTmpl NOTIFY signalMsgHtml)	

public:
	MsgHtmlObj(QObject* parent,QString msgLPicPath = "");	//发信息来的人的头像路径

	////替换初始化
	//QString msgLHtml() const { return m_msgLHtmlTmpl; }
	//QString msgRHtml() const { return m_msgRHtmlTmpl; }
	//
	//void setMsgLHtmlTmpl(const QString& tmpl) { m_msgLHtmlTmpl = tmpl; emit signalMsgHtml(tmpl); }
	//void setMsgRHtmlTmpl(const QString& tmpl) { m_msgRHtmlTmpl = tmpl; emit signalMsgHtml(tmpl); }

signals:
	void signalMsgHtml(const QString& html);

private:
	void initHtmlTmpl();//初始化聊天网页
	QString getMsgTmplHtml(const QString& code);
private:
	QString m_msgLPicPath;
	QString m_msgLHtmlTmpl;//别人发来的信息
	QString m_msgRHtmlTmpl;//自己发的信息
};

class MsgWebPage :public QWebEnginePage
{
	Q_OBJECT
		
public :
	MsgWebPage(QObject* parent = nullptr) :QWebEnginePage(parent) {}
protected:
	bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame);
};

class MsgWebView  : public QWebEngineView
{
	Q_OBJECT

public:
	MsgWebView(QWidget *parent);
	~MsgWebView();	
	void appendMsg(const QString& html,QString strObj = "0");
private slots:
	void onLoadFinished(bool ok);
private:
	QList<QStringList> parseHtml(const QString& html);	//解析html
	//Qt中所有Dom节点(属性,说明,文本)都可以使用QDomNode进行表示
	QList<QStringList> parseDocNode(const QDomNode& node);//解析节点
signals:
	void signalSendMsg(QString& strData, int& msgType, QString sFile = "");
private:
	MsgHtmlObj* m_msgHtmlObj;
	QWebChannel* m_channel;
};
