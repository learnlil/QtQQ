#include "MsgWebView.h"
#include <QFile>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QWebChannel>
#include <QSqlQueryModel>

#include "TalkWindowShell.h"
#include "WindowManager.h"

extern QString gstrLoginHeadPath;

MsgHtmlObj::MsgHtmlObj(QObject* parent,QString msgLPicPath) 
	:QObject(parent), m_msgLPicPath(msgLPicPath)
{
	initHtmlTmpl();
}

void MsgHtmlObj::initHtmlTmpl()
{
	m_msgLHtmlTmpl = getMsgTmplHtml("msgleftTmpl");
	m_msgLHtmlTmpl.replace("%1", m_msgLPicPath);

	m_msgRHtmlTmpl = getMsgTmplHtml("msgrightTmpl");
	m_msgRHtmlTmpl.replace("%1", gstrLoginHeadPath);
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
/// <summary>
/// 网页初始化QWeb,然后使用MsgHtmlObj来拼接网页内容,通过MsgWebView来显示内嵌的网页
/// </summary>
/// <param name="parent"></param>
MsgWebView::MsgWebView(QWidget *parent)
	: QWebEngineView(parent)
	,m_channel(new QWebChannel(this))
{
	MsgWebPage* page = new MsgWebPage(this);
	setPage(page);

	m_msgHtmlObj = new MsgHtmlObj(this);
	//绑定,JS文件中external0和m_msgHtmlObj关联起来,相当于 auto &external0 = m_msgHtmlObj.
	//再在js代码中external0 = channel.objects.external0;这样就可以在本地的js文件中调用m_msgHtmlObj了
	//并且在msgHtmlObj中的预定义,再js代码中可以调用external0.msgRHtmlTmpl
	m_channel->registerObject("external0",m_msgHtmlObj);
	
	TalkWindowShell* talkWindowShell = WindowManager::getInstance()->getTalkWindowShell();
	connect(this, &MsgWebView::signalSendMsg, talkWindowShell, &TalkWindowShell::updateSendTcpMsg);
	//test初始化收信息网页页面是否加载成功	成功
	//connect(this, &QWebEngineView::loadFinished, this, &MsgWebView::onLoadFinished);

	//当前正构建的聊天窗口的ID(QQ号)
	QString strTalkId = WindowManager::getInstance()->getCreatingTalkId();

	QSqlQueryModel queryEmployeeModel;
	QString strEmployeeID, strPicturePath;
	QString strExternal;
	bool isGroupTalk = false;

	//获取总群ID
	queryEmployeeModel.setQuery(QString("SELECT departmentID FROM tab_department WHERE departmentName = '%1'").arg(QStringLiteral("软工网211群")));
	QModelIndex companyIndex = queryEmployeeModel.index(0, 0);
	QString strCompanyID = queryEmployeeModel.data(companyIndex).toString();

	if (strTalkId == strCompanyID)//总群群聊
	{
		isGroupTalk = true;
		queryEmployeeModel.setQuery("SELECT employeeID,picture FROM tab_employee WHERE status = 1");
	}
	else
	{
		if (strTalkId.length() == 4)//其他群聊
		{
			isGroupTalk = true;
			queryEmployeeModel.setQuery(QString("SELECT employeeID,picture FROM tab_employee WHERE status = 1 AND departmentID = %1").arg(strTalkId));
		}
		else//单独聊天
		{
			queryEmployeeModel.setQuery(QString("SELECT picture FROM tab_employee WHERE status = 1 AND employeeID = %1").arg(strTalkId));
		
			QModelIndex index = queryEmployeeModel.index(0, 0);
			strPicturePath = queryEmployeeModel.data(index).toString();

			strExternal = "external_" + strTalkId;
			MsgHtmlObj* msgHtmlObj = new MsgHtmlObj(this, strPicturePath);
			m_channel->registerObject(strExternal,msgHtmlObj);
		}
	}

	if (isGroupTalk)
	{
		QModelIndex employeeModelIndex, pictureModelIndex;
		int rows = queryEmployeeModel.rowCount();
		for (int i = 0; i < rows; i++)
		{
			employeeModelIndex = queryEmployeeModel.index(i, 0);
			pictureModelIndex = queryEmployeeModel.index(i, 1);

			strEmployeeID = queryEmployeeModel.data(employeeModelIndex).toString();
			strPicturePath = queryEmployeeModel.data(pictureModelIndex).toString();

			strExternal = "external_" + strEmployeeID;

			MsgHtmlObj* msgHtmlObj = new MsgHtmlObj(this, strPicturePath);
			m_channel->registerObject(strExternal, msgHtmlObj);
		}
	}


	this->page()->setWebChannel(m_channel);
	//初始化收信息网页页面
	//之后js代码执行插入消息操作把消息插入到此html中并且刷新页面
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

	//格式:  text:[文字消息]
	//		 img :[表情路径]
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

			//如果表情是添加到资源文件的话,在html中瑶去掉qrc前缀
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
		/*发消息调用的js函数
		function appendHtml0(msg){
			if(external0 && external0.msgRHtmlTmpl)
			{
				$("#placeholder").append(external0.msgRHtmlTmpl.format(msg));
				window.scrollTo(0,document.body.scrollHeight);
			}
			else
			{
				console.error('报错');
				console.log(external0);
				console.log(external0.msgRHtmlTmpl);
			}
		};
		*/
		this->page()->runJavaScript(QString("appendHtml0(%1)").arg(Msg));
		if (isImageMsg)
		{
			strData = QString::number(imageNum) + "images"+strData;
		}
		//发信号执行updateSendTcpMsg,封装消息
		emit signalSendMsg(strData, msgType);
	}
	else//接收信息
	{
		this->page()->runJavaScript(QString("recvHtml_%1(%2)").arg(strObj).arg(Msg));
	}
	//	this->page()->runJavaScript(QString("appendHtml0(%1)").arg(Msg));
	//	emit signalSendMsg(strData, msgType);
}


/*
<?xml version="1.0"?>
<!DOCTYPE HTML SYSTEM "http://www.w3.org/TR/REC-html40/strict.dtd" PUBLIC "-//W3C//DTD HTML 4.0//EN">
<html>
	<head>
		<meta content="1" name="qrichtext"/>
		<meta charset="utf-8"/>
		<style type="text/css"> p, li { white-space: pre-wrap; } hr { height: 1px; border-width: 0; } li.unchecked::marker { content: "\2610"; } li.checked::marker { content: "\2612"; } </style>
	</head>
	<body style=" font-family:'Microsoft YaHei'; font-size:9pt; font-weight:400; font-style:normal;">
		<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
			<span style=" font-size:10pt;">12312234</span>
		</p>
	</body>
</html>
*/
QList<QStringList> MsgWebView::parseHtml(const QString & html)
{
	QDomDocument doc;
	doc.setContent(html);
	const QDomElement& root = doc.documentElement();//节点元素获得<html>...</html>
	const QDomElement& node = root.firstChildElement("body");//获得html中<body>...</body>,并不包含<body>

	return parseDocNode(node);
}
/*
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
	<span style=" font-size:10pt;">12312234</span>
</p>
*/
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
			//判断有没有孩子节点
			//第一轮调用只处理<p>标签,需要的是<p>下的<span>标签里的内容,递归插入<p>标签内容
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