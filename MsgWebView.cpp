#include "MsgWebView.h"
#include <QFile>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QWebChannel>

#include "TalkWindowShell.h"
#include "WindowManager.h"
MsgHtmlObj::MsgHtmlObj(QObject* parent) :QObject(parent)
{
	initHtmlTmpl();
}

void MsgHtmlObj::initHtmlTmpl()
{
	m_msgLHtmlTmpl = getMsgTmplHtml("msgleftTmpl");
	m_msgRHtmlTmpl = getMsgTmplHtml("msgrightTmpl");
}

QString MsgHtmlObj::getMsgTmplHtml(const QString& code)
{
	QFile file(":/Resources/MainWindow/MsgHtml/" + code + ".html");
	file.open(QFile::ReadOnly);
	QString strData;
	if (file.isOpen())
	{
		strData = QLatin1String(file.readAll());
	}
	else
	{
		QMessageBox::information(nullptr,"tips", "failed to init html");
		return QString(" ");
	}
	file.close();
	return strData;
}

MsgWebView::MsgWebView(QWidget *parent)
	: QWebEngineView(parent)
{
	MsgWebPage* page = new MsgWebPage(this);
	setPage(page);

	QWebChannel* channel = new QWebChannel(this);
	m_msgHtmlObj = new MsgHtmlObj(this);
	channel->registerObject("external",m_msgHtmlObj);
	this->page()->setWebChannel(channel);
	
	TalkWindowShell* talkWindowShell = WindowManager::getInstance()->getTalkWindowShell();
	connect(this, &MsgWebView::signalSendMsg, talkWindowShell, &TalkWindowShell::updateSendTcpMsg);
	//test初始化收信息网页页面是否加载成功	成功
	connect(this, &QWebEngineView::loadFinished, this, &MsgWebView::onLoadFinished);

	//初始化收信息网页页面
	this->load(QUrl("qrc:/Resources/MainWindow/MsgHtml/msgTmpl.html"));
}

MsgWebView::~MsgWebView()
{}

void MsgWebView::appendMsg(const QString & html, QString strObj)
{
	QJsonObject msgObj;
	QString qsMsg;
	const QList<QStringList> msgList = parseHtml(html);//解析html

	int imageNum = 0;
	int msgType = 1;//信息类型,0 表情 1 文本 2 文件
	bool isImageMsg = false;
	QString strData;//发送的数据 ,表情序号按照三位数,不足左侧补零 009 066 557


	for (int i = 0; i < msgList.size(); i++)
	{
		if (msgList.at(i).at(0) == "img")
		{
			QString imagePath = msgList.at(i).at(1);
			QPixmap pixmap;
			//获得表情名称所在位置
			QString strEmotionPath = "qrc:/Resources/MainWindow/emotion/";
			int pos = strEmotionPath.size();
			isImageMsg = true;

			//获取表情名称
			QString strEmotionName = imagePath.mid(pos);
			strEmotionName.replace(".png", "");

			//根据表情名称长度进行设置表情数据,不足三位补零
			int emotionNameL = strEmotionName.length();
			if (emotionNameL == 1)
			{
				strData = strData + "00" + strEmotionName;
			}
			else if (emotionNameL == 2)
			{
				strData = strData + "0" + strEmotionName;
			}
			else if (emotionNameL == 3)
			{
				strData = strData + strEmotionName;
			}
			msgType = 0;//表情标识
			imageNum++;

			if (imagePath.left(3) == "qrc")
			{
				pixmap.load(imagePath.mid(3));
			}
			else
				pixmap.load(imagePath);

			//表情图片html格式文本组合
			QString imgPath = QString("<img src=\"%1\" width = \"%2\" height = \"%3\"/>")
				.arg(imagePath).arg(pixmap.width()).arg(pixmap.height());
			qsMsg += imgPath;
		}
		else if (msgList.at(i).at(0) == "text")
		{
			qsMsg += msgList.at(i).at(1);
			strData = qsMsg;
		}

	}

	msgObj.insert("MSG", qsMsg);
	const QString& Msg = QJsonDocument(msgObj).toJson(QJsonDocument::Compact);
	if (strObj == "0")//发信息
	{
		this->page()->runJavaScript(QString("appendHtml0(%1)").arg(Msg));
		if (isImageMsg)
		{
			strData = QString::number(imageNum) + "images"+strData;
		}
		emit signalSendMsg(strData, msgType);
	}
	else//接收信息
	{
		this->page()->runJavaScript(QString("recvHtml_%1(%2)").arg(strObj).arg(Msg));
	}
	//	this->page()->runJavaScript(QString("appendHtml0(%1)").arg(Msg));
	//	emit signalSendMsg(strData, msgType);
}

QList<QStringList> MsgWebView::parseHtml(const QString & html)
{
	QDomDocument doc;
	doc.setContent(html);
	const QDomElement& root = doc.documentElement();//节点元素
	const QDomElement& node = root.firstChildElement("body");

	return parseDocNode(node);
}

QList<QStringList> MsgWebView::parseDocNode(const QDomNode& node)
{
	QList<QStringList> attribute;
	const QDomNodeList& list = node.childNodes();//返回所有子节点
	
	for (int i = 0; i < list.count(); i++)
	{
		const QDomNode& node = list.at(i);
		if (node.isElement())
		{
			//转换元素
			const QDomElement& element = node.toElement();
			if (element.tagName() == "img")
			{
				QStringList attributeList;
				attributeList << "img" << element.attribute("src");
				attribute << attributeList;
			}
			if (element.tagName() == "span")
			{
				QStringList attributeList;
				attributeList << "text" << element.text();
				attribute << attributeList;
			}
			if (node.hasChildNodes())
			{
				attribute<<parseDocNode(node);
			}
		}
	}

	return attribute;
}

bool MsgWebPage::acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)
{
	//仅接受qrc:/*.html
	if (url.scheme() == QString("qrc"))//判断url类型
	{
		return true;
	}else
		return false;
} 

void MsgWebView::onLoadFinished(bool ok)
{
	if (ok)
	{
		QMessageBox::information(this, "成功", QString(this->url().toString())+" "+QString(this->title()));
	}
	else
	{
		QMessageBox::information(this, "失败", "失败");
	}
}