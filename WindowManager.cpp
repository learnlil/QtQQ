#include "WindowManager.h"
#include "TalkWindow.h"
#include "TalkWindowItem.h"

//单例模式.创建全局静态对象
Q_GLOBAL_STATIC(WindowManager,theInstance)

WindowManager::WindowManager()
	:QObject(nullptr)
	,m_talkwindowshell(nullptr)
{

}

WindowManager::~WindowManager()
{

}

QWidget* WindowManager::findWindowName(const QString& qsWindowName)
{
	if (m_windowMap.contains(qsWindowName))
	{
		return m_windowMap.value(qsWindowName);
	}
	return nullptr;
}

void WindowManager::deleteWindowName(const QString& qsWindowName)
{
	m_windowMap.remove(qsWindowName);
}

void WindowManager::addWindowName(const QString& qsWindowName, QWidget* qWidget)
{
	if (!m_windowMap.contains(qsWindowName))
	{
		m_windowMap.insert(qsWindowName, qWidget);
	}
}

WindowManager* WindowManager::getInstance()
{

	return theInstance();
}

void WindowManager::addNewTalkWindow(const QString& uid, GroupType groupType, const QString& strPeople)
{
	if (m_talkwindowshell == nullptr)
	{
		m_talkwindowshell = new TalkWindowShell;
		connect(m_talkwindowshell, &TalkWindowShell::destroyed, [this](QObject* obj)
			{
				m_talkwindowshell = nullptr;
			});
	}

	QWidget* widget = findWindowName(uid);
	if (!widget)
	{
		TalkWindow* talkwindow = new TalkWindow(m_talkwindowshell, uid, groupType);
		TalkWindowItem* talkwindowItem = new TalkWindowItem(talkwindow);

		switch (groupType) {
		case COMPANY:
		{
			talkwindow->setWindowName(QStringLiteral("软工网212李立群"));
			talkwindowItem->setMsgLabelContent(QStringLiteral("软工网212群"));
			break;
		}
		case PERSONELGROUP:
		{
			talkwindow->setWindowName(QStringLiteral("富强,民主,文明,和谐"));
			talkwindowItem->setMsgLabelContent(QStringLiteral("富强群"));
			break;
		}
		case MAKETGROUP:
		{
			talkwindow->setWindowName(QStringLiteral("今天学习努把力,明天努力找工作"));
			talkwindowItem->setMsgLabelContent(QStringLiteral("steam"));
			break;
		}
		case DEVELOPMENT:
		{
			talkwindow->setWindowName(QStringLiteral("编程语言狗都不学"));
			talkwindowItem->setMsgLabelContent(QStringLiteral("学习群"));
			break;
		}
		case PTOP:
		{
			talkwindow->setWindowName(QStringLiteral("李在干什么"));
			talkwindowItem->setMsgLabelContent(QStringLiteral("私聊"));
			break;
		}
		default:
			break;
		}
		m_talkwindowshell->addTalkWindow(talkwindow, talkwindowItem, groupType);
	}
	else
	{
		//左侧聊天列表设为选中
		QListWidgetItem* item = m_talkwindowshell->getTalkWindowItemMap().key(widget);
		item->setSelected(true);

		//设置右侧当前聊天窗口
		m_talkwindowshell->setCurrentWidget(widget);
	}

	m_talkwindowshell->show();
	m_talkwindowshell->activateWindow();
}
