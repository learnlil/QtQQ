#include "TalkWindow.h"

TalkWindow::TalkWindow(QWidget *parent,const QString& uid,GroupType grouptype)
	: QWidget(parent)
	,m_talkId(uid)
	,m_groupType(grouptype)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initControl();
}

TalkWindow::~TalkWindow()
{}

void TalkWindow::addEmotionImage(int emotionNum)
{
}

void TalkWindow::setWindowName(const QString& name)
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
}

void TalkWindow::initCompanyTalk()
{
}

void TalkWindow::initPersonelTalk()
{
}

void TalkWindow::initMaketTalk()
{
}

void TalkWindow::initDevelopTalk()
{
}
 
void TalkWindow::onSendBtnClicked(bool)
{

}