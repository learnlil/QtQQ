#include "TalkWindow.h"
#include "RootContatItem.h"
#include "ContactItem.h"
#include "CommonUtils.h"
#include "WindowManager.h"

TalkWindow::TalkWindow(QWidget *parent,const QString& uid,GroupType grouptype)
	: QWidget(parent)
	,m_talkId(uid)
	,m_groupType(grouptype)
{
	ui.setupUi(this);
	WindowManager::getInstance()->addWindowName(m_talkId, this);
	setAttribute(Qt::WA_DeleteOnClose);
	initControl();
}

TalkWindow::~TalkWindow()
{
	WindowManager::getInstance()->deleteWindowName(m_talkId);
}

void TalkWindow::addEmotionImage(int emotionNum)
{
}

void TalkWindow::setWindowName(const QString& name)
{
	ui.nameLabel->setText(name);
}

void TalkWindow::onItemDoubleClicked()
{

}

void TalkWindow::initControl()
{
	QList<int> rightWidgetSize;
	rightWidgetSize << 600 << 138;
	ui.bodySplitter->setSizes(rightWidgetSize);

	ui.textEdit->setFontPointSize(10);
	ui.textEdit->setFocus();

	connect(ui.sysmin, SIGNAL(clicked(bool)), parent(), SLOT(onShowMin(bool)));
	connect(ui.sysclose, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));
	connect(ui.closeBtn, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));

	connect(ui.faceBtn, SIGNAL(clicked(bool)), parent(), SLOT(onEmotionBtnClicked(bool)));
	connect(ui.sendBtn, SIGNAL(clicked(bool)), this, SLOT(onSendBtnClicked(bool)));
	
	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));

	switch (m_groupType)
	{
	case COMPANY:
	{
		initCompanyTalk();
		break;
	}
	case PERSONELGROUP:
	{
		initPersonelTalk();
		break;
	}
	case MAKETGROUP:
	{
		initMaketTalk();
		break;
	}
	case DEVELOPMENT:
	{
		initDevelopTalk();
		break;
	}
	case PTOP:
	{
		initPtoPTalk();
		break;
	}
	default:
		break;
	}
}

void TalkWindow::initPtoPTalk()
{
	QPixmap pixSkin;
	pixSkin.load(":/Resources/MainWindow/skin.png");

	ui.widget->setFixedSize(pixSkin.size());

	QLabel* skinLabel = new QLabel(ui.widget);
	skinLabel->setPixmap(pixSkin);
	skinLabel->setFixedSize(ui.widget->size());
	
}

void TalkWindow::initCompanyTalk()
{
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

	//设置Data,用去区分根项,子项
	pRootItem->setData(0, Qt::UserRole, 0);
	RootContatItem* pItemName = new RootContatItem(false, ui.treeWidget);

	ui.treeWidget->setFixedHeight(646);//shell高度-shell头高(talkWindow titleWidget.height)

	int nEmployeeNum = 50;
	QString qsGroupName = QString::fromUtf8("软工网211群 %1/%2").arg(0).arg(nEmployeeNum);
	pItemName->setText(qsGroupName);

	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);
	ui.treeWidget->setItemWidget(pRootItem,0,pItemName);

	//展开
	pRootItem->setExpanded(true);

	for (int i = 0; i < nEmployeeNum; i++)
	{
		addPeopInfo(pRootItem);
	}
}

void TalkWindow::initPersonelTalk()
{
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

	//设置Data,用去区分根项,子项
	pRootItem->setData(0, Qt::UserRole, 0);
	RootContatItem* pItemName = new RootContatItem(false, ui.treeWidget);

	ui.treeWidget->setFixedHeight(646);//shell高度-shell头高(talkWindow titleWidget.height)

	int nEmployeeNum = 5;
	QString qsGroupName = QString::fromUtf8("软工网212群 %1/%2").arg(0).arg(nEmployeeNum);
	pItemName->setText(qsGroupName);

	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);

	//展开
	pRootItem->setExpanded(true);

	for (int i = 0; i < nEmployeeNum; i++)
	{
		addPeopInfo(pRootItem);
	}
}

void TalkWindow::initMaketTalk()
{
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

	//设置Data,用去区分根项,子项
	pRootItem->setData(0, Qt::UserRole, 0);
	RootContatItem* pItemName = new RootContatItem(false, ui.treeWidget);

	ui.treeWidget->setFixedHeight(646);//shell高度-shell头高(talkWindow titleWidget.height)

	int nEmployeeNum = 8;
	QString qsGroupName = QString::fromUtf8("软工网213群 %1/%2").arg(0).arg(nEmployeeNum);
	pItemName->setText(qsGroupName);

	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);

	//展开
	pRootItem->setExpanded(true);

	for (int i = 0; i < nEmployeeNum; i++)
	{
		addPeopInfo(pRootItem);
	}
}

void TalkWindow::initDevelopTalk()
{
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

	//设置Data,用去区分根项,子项
	pRootItem->setData(0, Qt::UserRole, 0);
	RootContatItem* pItemName = new RootContatItem(false, ui.treeWidget);

	ui.treeWidget->setFixedHeight(646);//shell高度-shell头高(talkWindow titleWidget.height)

	int nEmployeeNum = 32;
	QString qsGroupName = QString::fromUtf8("软工网214群 %1/%2").arg(0).arg(nEmployeeNum);
	pItemName->setText(qsGroupName);

	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);

	//展开
	pRootItem->setExpanded(true);

	for (int i = 0; i < nEmployeeNum; i++)
	{
		addPeopInfo(pRootItem);
	}
}

void TalkWindow::addPeopInfo(QTreeWidgetItem* pRootGroupItem)
{
	QTreeWidgetItem* pChild = new QTreeWidgetItem();

	QPixmap pix1;
	pix1.load(":/Resources/MainWindow/head_mask.png");
	QImage image(":/Resources/MainWindow/girl.png");

	//添加子节点
	pChild->setData(0, Qt::UserRole, 1);
	pChild->setData(0, Qt::UserRole + 1, QString::number((int)pChild));
	ContactItem* pContactItem = new ContactItem(ui.treeWidget);

	static int i = 0;
	pContactItem->setHeadPixmap(CommonUtils::getRoundImage(QPixmap::fromImage(image),pix1,pContactItem->getHeadLabelSize()));
	pContactItem->setUserName(QString::fromUtf8("李立群-%1").arg(i++));
	pContactItem->setSignName(QString::fromUtf8("%1").arg(i-1));

	pRootGroupItem->addChild(pChild);
	ui.treeWidget->setItemWidget(pChild, 0, pContactItem);

	QString str = pContactItem->getUserName();
	m_groupPeopleMap.insert(pChild, str);

}
 
void TalkWindow::onSendBtnClicked(bool)
{

}