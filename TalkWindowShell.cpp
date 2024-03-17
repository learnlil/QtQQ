#include "TalkWindowShell.h"
#include "CommonUtils.h"
#include "EmotionWindow.h"
#include "TalkWindow.h"
#include "TalkWindowItem.h"
#include "WindowManager.h"
#include "ReceiveFile.h"

#include <QListWidget>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QFile>
#include <QSqlQuery>

QString gfileName;			//文件名称
QString gfileData;			//文件内容
extern QString gLoginEmployeeID;
const int gudpPort = 6666;

TalkWindowShell::TalkWindowShell(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initControl();
	initTcpSocket();
	initUdpSocket();
	

	QFile file("Resources/MainWindow/MsgHtml/msgtmpl.js");
	if (!file.size())
	{
		QStringList employeesIDList;
		getEmployeesID(employeesIDList);
		if (!createJSFile(employeesIDList))
		{
			QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("更新js文件数据失败"));
		}
	}
	
}

TalkWindowShell::~TalkWindowShell()
{
	delete m_emotionWindow;
	m_emotionWindow = nullptr;
}

void TalkWindowShell::addTalkWindow(TalkWindow* talkWindow, TalkWindowItem* talkWindowItem,const QString& uid)//GroupType gouptype)
{
	ui.rightStackedWidget->addWidget(talkWindow);
	connect(m_emotionWindow, SIGNAL(signalEmotionWindowHide()),
		talkWindow, SLOT(onSetEmotionBtnStatus()));

	QListWidgetItem* aItem = new QListWidgetItem(ui.listWidget);
	m_talkWindowItemMap.insert(aItem, talkWindow);

	aItem->setSelected(true);

	//判断是群聊还是单聊
	QSqlQueryModel sqlDepModel;
	QString strQuery = QString("SELECT picture FROM tab_department WHERE departmentID = %1").arg(uid);
	sqlDepModel.setQuery(strQuery);
	int rows = sqlDepModel.rowCount();

	if (rows == 0)
	{
		strQuery = QString("SELECT picture FROM tab_employee WHERE employeeID = %1").arg(uid);
		sqlDepModel.setQuery(strQuery);
	}
	QModelIndex index;
	index = sqlDepModel.index(0, 0);	//行,列获得头像路径
	QImage img;
	img.load(sqlDepModel.data(index).toString());

	talkWindowItem->setHeadPixmap(QPixmap::fromImage(img));//(":/Resources/MainWindow/girl.png");//设置头像,之后修改
	
	ui.listWidget->addItem(aItem);
	ui.listWidget->setItemWidget(aItem, talkWindowItem);

	onTalkWindowItemClicked(aItem);

	connect(talkWindowItem, &TalkWindowItem::signalCloseClicked,
		[talkWindowItem, talkWindow, aItem, this]()
		{
			m_talkWindowItemMap.remove(aItem);
			talkWindow->close();
			ui.listWidget->takeItem(ui.listWidget->row(aItem));
			delete talkWindowItem;
			ui.rightStackedWidget->removeWidget(talkWindow);
			if (ui.rightStackedWidget->count() < 1)
				close();
		});
}

void TalkWindowShell::setCurrentWidget(QWidget* widget)
{
	ui.rightStackedWidget->setCurrentWidget(widget);
}

const QMap<QListWidgetItem*, QWidget*>& TalkWindowShell::getTalkWindowItemMap() const
{
	return m_talkWindowItemMap;
}

void TalkWindowShell::onEmotionBtnClicked(bool)
{
	m_emotionWindow->setVisible(!m_emotionWindow->isVisible());
	QPoint emotionPoint = this->mapToGlobal(QPoint(0, 0));//将当前控件的相对位置转换为屏幕的绝对位置
	
	emotionPoint.setX(emotionPoint.x() + 170);
	emotionPoint.setY(emotionPoint.y() + 220);
	m_emotionWindow->move(emotionPoint);

}

void TalkWindowShell::onTalkWindowItemClicked(QListWidgetItem* item)
{
	QWidget* talkwindowWidget = m_talkWindowItemMap.find(item).value();
	ui.rightStackedWidget->setCurrentWidget(talkwindowWidget);
}

void TalkWindowShell::initControl()
{
	loadStyleSheet("TalkWindow");
	setWindowTitle(QString::fromUtf8("聊天窗口"));

	m_emotionWindow = new EmotionWindow;
	m_emotionWindow->hide();

	QList<int> leftWidgetSize;
	leftWidgetSize << 154 << width() - 154;
	ui.splitter->setSizes(leftWidgetSize);	//分裂器进行尺寸划分

	ui.listWidget->setStyle(new CustomProxyStyle(this));

	connect(ui.listWidget, &QListWidget::itemClicked, this, &TalkWindowShell::onTalkWindowItemClicked);
	connect(m_emotionWindow, SIGNAL(signalEmotionItemClicked(int)), this, SLOT(onEmotionItemClicked(int)));


}

void TalkWindowShell::initTcpSocket()
{
	m_tcpClientSocket = new QTcpSocket(this);
	m_tcpClientSocket->connectToHost("127.0.0.1", gTcpPort);
}

void TalkWindowShell::initUdpSocket()
{
	m_udpReceiver = new QUdpSocket(this);
	for (quint16 port = gudpPort; port < gudpPort + 200; port++)
	{
		if (m_udpReceiver->bind(port, QUdpSocket::ShareAddress))
		{
			break;
		}
	}
	connect(m_udpReceiver, &QUdpSocket::readyRead, this, &TalkWindowShell::processPendingData);
}

void TalkWindowShell::getEmployeesID(QStringList& employeesList)
{
	QSqlQueryModel queryModel;
	queryModel.setQuery("SELECT employeeID FROM tab_employee WHERE status=1");
	
	//返回模型的总行数(员工的总数)
	int employeesNum = queryModel.rowCount();
	QModelIndex index;
	for (int i = 0; i < employeesNum; ++i)
	{
		index = queryModel.index(i, 0);
		employeesList<<queryModel.data(index).toString();

	}
}

bool TalkWindowShell::createJSFile(QStringList& employeesList)
{
	//读取txt文件数据
	QString strFileTxt = "Resources/MainWindow/MsgHtml/msgtmpl.txt";
	QFile fileRead(strFileTxt);
	QString strFile;

	if (fileRead.open(QIODevice::ReadOnly))
	{
		strFile = fileRead.readAll();
		fileRead.close();
	}
	else
	{
		QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("读取文本文件msgtmpl.txt失败"));
		return false;
	}

	//替换(external0,appendHtml0用作自己发信息使用)
	QFile fileWrite("Resources/MainWindow/MsgHtml/msgtmpl.js");
	if (fileWrite.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		//更新空值
		QString strSourceInitNull = "var external = null;";
		//更新初始化
		QString strSourceInit = "external = channel.objects.external;";
		//更新newWebChannel
		QString strSourceNew =
			"new QWebChannel(qt.webChannelTransport,\
			function(channel) {\
			external = channel.objects.external;\
		}\
		); ";

		//更新追加recvHtml,脚本中有双引号无法直接进行字符串赋值
		QString strSourceRecvHtml;
		QFile fileRecvHtml("Resources/MainWindow/MsgHtml/recvHtml.txt");
		if (fileRecvHtml.open(QIODevice::ReadOnly))
		{
			strSourceRecvHtml = fileRecvHtml.readAll();
			fileRecvHtml.close();
		}
		else
		{
			QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("读取文本文件recvHtml.txt失败"));
			return false;
		}

		//保存替换后的脚本
		QString strReplaceInitNull;
		QString strReplaceInit;
		QString strReplaceNew;
		QString strReplaceRecvHtml;

		for (int i = 0; i < employeesList.length(); i++)
		{
			//编辑替换后的空值
			QString strInitNull = strSourceInitNull;
			strInitNull.replace("external", QString("external_%1").arg(employeesList.at(i)));
			strReplaceInitNull += strInitNull;
			strReplaceInitNull += "\n";

			//编辑替换后的初始值
			QString strInit = strSourceInit;
			strInit.replace("external", QString("external_%1").arg(employeesList.at(i)));
			strReplaceInit += strInit;
			strReplaceInit += "\n";

			//编辑替换后的newWebChannel
			QString strNew = strSourceNew;
			strNew.replace("external", QString("external_%1").arg(employeesList.at(i)));
			strReplaceNew += strNew;
			strReplaceNew += "\n";

			//编辑替换后的recvHtml
			QString strRecvHtml = strSourceRecvHtml;
			strRecvHtml.replace("external", QString("external_%1").arg(employeesList.at(i)));
			strRecvHtml.replace("recvHtml", QString("recvHtml_%1").arg(employeesList.at(i)));
			strReplaceRecvHtml += strRecvHtml;
			strReplaceRecvHtml += "\n";
		}

		strFile.replace(strSourceInitNull, strReplaceInitNull);
		strFile.replace(strSourceInit, strReplaceInit);
		strFile.replace(strSourceNew, strReplaceNew);
		strFile.replace(strSourceRecvHtml, strReplaceRecvHtml);

		QTextStream stream(&fileWrite);
		stream << strFile;
		fileWrite.close();
		return true;
	}
	else
	{
		QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("写js脚本msghtml.js失败"));
		return false;
	}
}

void TalkWindowShell::handleReceiveMsg(int senderEmployeeID, int msgType, QString strMsg)
{
	QMsgTextEdit msgTextEdit;
	msgTextEdit.setText(strMsg);

	if (msgType == 1)//文本信息
	{
		msgTextEdit.document()->toHtml();
	}
	else if (msgType == 0)//表情信息
	{
		const int emotionWidth = 3;
		int emotionNum = strMsg.length() / emotionWidth;
		for (int i = 0; i < emotionNum; i++)
		{
			msgTextEdit.addEmotionUrl(strMsg.mid(i * emotionWidth, emotionWidth).toInt());
		}
	}

	QString html = msgTextEdit.document()->toHtml();

	//文本Html如果没有字体则添加字体
	if (!html.contains(".png") && !html.contains("</span>"))
	{
		QString fontHtml;
		QFile file(":/Resources/MainWindow/MsgHtml/msgFont.txt");
		if (file.open(QIODevice::ReadOnly))
		{
			fontHtml = file.readAll();
			fontHtml.replace("%1", strMsg);
			file.close();
		}
		else
		{
			QMessageBox::information(this, QString::fromUtf8("提示"),
				QString::fromUtf8("未找到文件msgFont"));
			return;
		}
		if (!html.contains(fontHtml))
		{
			html.replace(strMsg, fontHtml);
		}
	}

	TalkWindow* talkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	talkWindow->ui.msgWidget->appendMsg(html, QString::number(senderEmployeeID));

}

//文本数据包格式:群聊标志+发信息员工QQ号+收信息员工QQ号(群QQ号)+信息类型+数据长度+数据
//表情数据包格式:群聊标志+发信息员工QQ号+收信息员工QQ号(群QQ号)+信息类型+表情个数+images+数据
//msgType 0表情 1文本信息 2文件信息
void TalkWindowShell::updateSendTcpMsg(QString& strData, int& msgType, QString fileName)
{
	//获取当前活动聊天窗口
	TalkWindow* curTalkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	QString talkId = curTalkWindow->getTalkId();

	QString strGroupFlag;
	QString strSend;

	if (talkId.length() == 4)//群QQ号的长度
	{
		strGroupFlag = "1";
	}
	else
	{
		strGroupFlag = "0";
	}

	int nstrDataLength = strData.length();
	int dataLength = QString::number(nstrDataLength).length();
//	const int sourceDataLenth = dataLength;
	QString strDataLength;
	if (msgType == 1)//发送文本信息
	{
		//文本信息的长度约定为5位
		if (dataLength == 1)//hello 5==1 50 == 2 500 == 3 5000 == 4 50000 == 5
		{
			strDataLength = "0000" + QString::number(nstrDataLength);
		}
		else if (dataLength == 2)
		{
			strDataLength = "000" + QString::number(nstrDataLength);
		}
		else if (dataLength == 3)
		{
			strDataLength = "00" + QString::number(nstrDataLength);
		}
		else if (dataLength == 4)
		{
			strDataLength = "0" + QString::number(nstrDataLength);
		}
		else if (dataLength == 5)
		{
			strDataLength =  QString::number(nstrDataLength);
		}
		else
		{
			QMessageBox::information(this,QString::fromUtf8( "提示"),QString::fromUtf8( "发送的文本数据长度过长"));
		}
		//文本数据包格式:群聊标志+发信息员工QQ号+收信息员工QQ号(群QQ号)+信息类型+数据长度+数据
		strSend = strGroupFlag + gLoginEmployeeID + talkId + "1" + strDataLength + strData;
	}
	//表情信息
	else if (msgType == 0)
	{
		//表情数据包格式:群聊标志+发信息员工QQ号+收信息员工QQ号(群QQ号)+信息类型+表情个数+images+数据
		strSend = strGroupFlag + gLoginEmployeeID + talkId + "0" + strDataLength + strData;
	}
	//文件信息
	else if (msgType == 2)
	{
		//文件数据包格式:群聊标志+发信息员工QQ号+收信息员工QQ号(群QQ号)+信息类型+文件长度+"bytes"+文件名称+"data_begin"+文件内容
		//QByteArray
		QByteArray bt = strData.toUtf8();
		QString strLength = QString::number(bt.length());
		strSend = strGroupFlag + gLoginEmployeeID + talkId + "2" + strLength + "bytes" + fileName + "data_begin" + strData;
	}

	QByteArray dataBt;
	dataBt.resize(strSend.length());
	dataBt = strSend.toUtf8();
	m_tcpClientSocket->write(dataBt);
}

void TalkWindowShell::onEmotionItemClicked(int emotionNum)
{
	TalkWindow* currentTalkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	if (currentTalkWindow)
	{
		currentTalkWindow->addEmotionImage(emotionNum);
	}

}

/*
	数据包格式
	文本数据包格式:群聊标志+发信息QQ号+收信息QQ号(个人或群)+信息类型(1)+数据长度+数据
	表情数据包格式:群聊标志+发信息QQ号+收信息QQ号(个人或群)+信息类型(0)+表情个数+images+表情名称
	文件数据包格式:群聊标志+发信息QQ号+收信息QQ号(个人或群)+信息类型(2)+文件字节数+bytes+文件名+data_begin+文件数据

	群聊标志一位,0单聊,1群聊
	为1时没有收信息个人QQ号,而是收信息群QQ号
	为0时没有收信息群QQ号,而是收信息个人QQ号
	信息类型一位,0表情信息,1文本信息,2文件信息
	
	QQ号占5位,群QQ号4位,数据长度占5位,表情名称3位
	
	群聊文本信息:1100012001100005helloworld 表示QQ10001向群2001发送文本,长度为00005 ,helloword
	单聊图片信息:0100011000201images060            表示QQ10001向人10002发送一个表情,60.png
	群聊文件信息:1100052000210bytestest.txtdata_beginhelloworld
											表示QQ10005向群2000发送文件信息,文件内容长度10文件名test.txt,内容helloworld
	
	群聊文本信息解析: 1 10001 2001  1 00005 helloworld
	单聊图片信息解析: 0 10001 10002 0 1 images 060
	群聊文件信息解析: 1 10005 2000  10 bytes test.txt data_begin helloworld
*/
void TalkWindowShell::processPendingData()
{
	//端口中有未处理的数据
	while (m_udpReceiver->hasPendingDatagrams())
	{
		const static int groupFlagWidth = 1;		//群聊标志占位
		const static int groupWidth = 4;			//群QQ号宽度
		const static int employeeWidth = 5;			//人QQ号宽度
		const static int msgTypeWidth = 1;			//信息类型宽度
		const static int msgLengthWidth = 5;		//文本信息长度的宽度
		const static int pictureWidth = 3;			//表情图片的宽度

		//读取udp数据
		QByteArray btData;
		btData.resize(m_udpReceiver->pendingDatagramSize());
		m_udpReceiver->readDatagram(btData.data(), btData.size());

		QString strData = btData.data();
		QString strWindowID;	//聊天窗口ID,群聊则是群号,单聊则是员工QQ号
		QString strSendEmployeeID, strReceiveEmployeeID;	//发送即接收端QQ号
		QString strMsg;			//数据

		int msgLen;				//数据长度
		int msgType;			//数据类型

		strSendEmployeeID = strData.mid(groupFlagWidth, employeeWidth);

		//自己发的信息不做处理
		if (strSendEmployeeID == gLoginEmployeeID)
		{
			return;
		}

		if (btData[0] == '1')//群聊
		{
			//群QQ号
			strWindowID = strData.mid(groupFlagWidth + employeeWidth, groupWidth);

			QChar cMsgType = btData[groupFlagWidth + employeeWidth + groupWidth];
			if (cMsgType == '1')//文本信息
			{
				msgType = 1;
				msgLen = strData.mid(groupFlagWidth + employeeWidth
					+ groupWidth + msgTypeWidth, msgLengthWidth).toInt();
				strMsg = strData.mid(groupFlagWidth + employeeWidth
					+ groupWidth + msgTypeWidth+msgLengthWidth,msgLen);
			}
			else if (cMsgType == '0')//表情信息
			{
				msgType = 0;
				int posImages = strData.indexOf("images");
				strMsg = strData.right(strData.length() - posImages - QString("images").length());

			}
			else if(cMsgType == '2')//文件信息
			{
				msgType = 2;
				int bytesWidth = QString("bytes").length();
				int posBytes = strData.indexOf("bytes");
				int posData_begin = strData.indexOf("data_begin");
				//获取文件名称
				QString fileName = strData.mid(posBytes + bytesWidth, posData_begin - posBytes - bytesWidth);
				gfileName = fileName;

				//文件内容
				int dataLengthWidth;
				int posData = posData_begin+QString("data_begin").length();
				strMsg = strData.mid(posData);
				gfileData = strMsg;

				//根据employeeID获取发送者姓名
				QString sender;
				int employeeID = strSendEmployeeID.toInt();
				QString sqlQueryGroupName(QString("SELECT employeeName FROM tab_employee WHERE employeeID = %1")
					.arg(employeeID));
				QSqlQuery querySenderName;
				querySenderName.prepare(sqlQueryGroupName);
				querySenderName.exec();
				if (querySenderName.first())
				{
					sender = querySenderName.value(0).toString();
				}

				ReceiveFile* receiveFile = new ReceiveFile(this);
				connect(receiveFile, &ReceiveFile::refuseFile, [this]() {
					return;
				});
				QString msgLabel = QString::fromUtf8("收到来自") + sender
					+ QString::fromUtf8("发来的文件,是否接收?");
				receiveFile->setMsg(msgLabel);
				receiveFile->show();
			}
		}
		else//单聊
		{

			strReceiveEmployeeID = strData.mid(groupFlagWidth + employeeWidth, employeeWidth);
			strWindowID = strReceiveEmployeeID;

			//不是发给自己的信息不做处理
			if (strReceiveEmployeeID != gLoginEmployeeID)
			{
				return;
			}

			//获取信息类型
			QChar cMsgType = btData[groupFlagWidth + employeeWidth + employeeWidth];
			if (cMsgType == '1')//文本信息
			{
				msgType = 1;

				//文本信息长度
				msgLen = strData.mid(groupFlagWidth + employeeWidth + employeeWidth
					+ msgTypeWidth, msgLengthWidth).toInt();
				//文本信息
				strMsg = strData.mid(groupFlagWidth + employeeWidth + employeeWidth
					+ msgTypeWidth + msgLengthWidth, msgLen);
			}
			else if (cMsgType == '0')//表情信息
			{
				msgType = 0;
				int posImages = strData.indexOf("images");
				int imagesWidth = QString("images").length();
				strMsg = strData.mid(posImages + imagesWidth);

			}
			else if (cMsgType == '2')//文件信息
			{
				msgType = 2;

				int bytesWidth = QString("bytes").length();
				int posBytes = strData.indexOf("bytes");
				int data_beginWidth = QString("data_begin").length();
				int posData_begin = strData.indexOf("data_begin");

				//文件名称
				QString fileName = strData.mid(posBytes + bytesWidth, posData_begin - posBytes - bytesWidth);
				gfileName = fileName;
				//文件内容
				strMsg = strData.mid(posData_begin + data_beginWidth);
				gfileData = strMsg;

				//根据employeeID获取发送者姓名
				QString sender;
				int employeeID = strSendEmployeeID.toInt();
				QString sqlQueryGroupName(QString("SELECT employeeName FROM tab_employee WHERE employeeID = %1")
					.arg(employeeID));
				QSqlQuery querySenderName;
				querySenderName.prepare(sqlQueryGroupName);
				querySenderName.exec();
				if (querySenderName.first())
				{
					sender = querySenderName.value(0).toString();
				}

				ReceiveFile* receiveFile = new ReceiveFile(this);
				connect(receiveFile, &ReceiveFile::refuseFile, [this]() {
					return;
					});
				QString msgLabel = QString::fromUtf8("收到来自") + sender
					+ QString::fromUtf8("发来的文件,是否接收?");
				receiveFile->setMsg(msgLabel);
				receiveFile->show();
			}
		}

		//将聊天窗口设为活动的窗口
		QWidget* widget = WindowManager::getInstance()->findWindowName(strWindowID);
		if (widget)	//聊天窗口存在
		{
			this->setCurrentWidget(widget);

			//同步激活左侧聊天窗口
			QListWidgetItem* item = m_talkWindowItemMap.key(widget);
			item->setSelected(true);
		}
		else//聊天窗口未打开
		{
			return; 
		}
		//文件信息另做处理
		if (msgType != 2)
		{
			int sendEmployeeID = strSendEmployeeID.toInt();
			handleReceiveMsg(sendEmployeeID, msgType, strMsg);

		}
	}
}
