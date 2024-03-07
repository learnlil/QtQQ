#include "SkinWindow.h"

SkinWindow::SkinWindow(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	initControl();
}

SkinWindow::~SkinWindow()
{
	
}

void SkinWindow::initControl()
{
	QList<QColor> colorList = {
		QColor(22,154,218),QColor(40,138,221),QColor(48,166,107),QColor(218,67,68),
		QColor(177,99,158),QColor(107,81,92),QColor(89,92,160),QColor(21,156,199),
		QColor(79,169,172),QColor(155,183,154),QColor(128,77,77),QColor(240,188,189)
	};
	for (int row = 0; row < 3; row++)
	{

	}
}
