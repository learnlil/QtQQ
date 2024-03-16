#include "TalkWindowShell.h"
#include "CommonUtils.h"
#include "EmotionWindow.h"
#include "TalkWindow.h"
#include "TalkWindowItem.h"

#include <QListWidget>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QFile>

extern QString gLoginEmployeeID;

TalkWindowShell::TalkWindowShell(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initControl();
	initTcpSocket();

	

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

	int dataLength = strData.length();
	const int sourceDataLenth = dataLength;
	QString strDataLength;
	if (msgType == 1)//发送文本信息
	{
		//文本信息的长度约定为5位
		if (dataLength == 1)//hello 5==1 50 == 2 500 == 3 5000 == 4 50000 == 5
		{
			strDataLength = "0000" + QString::number(sourceDataLenth);
		}
		else if (dataLength == 2)
		{
			strDataLength = "000" + QString::number(sourceDataLenth);
		}
		else if (dataLength == 3)
		{
			strDataLength = "00" + QString::number(sourceDataLenth);
		}
		else if (dataLength == 4)
		{
			strDataLength = "0" + QString::number(sourceDataLenth);
		}
		else if (dataLength == 5)
		{
			strDataLength =  QString::number(sourceDataLenth);
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
