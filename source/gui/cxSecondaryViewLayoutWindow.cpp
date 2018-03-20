/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSecondaryViewLayoutWindow.h"


#include <QApplication>
#include <QDesktopWidget>
#include "cxTypeConversions.h"
#include "cxViewService.h"
#include "cxLogger.h"
#include "cxLogicManager.h"

namespace cx
{

SecondaryViewLayoutWindow::SecondaryViewLayoutWindow(QWidget* parent) :
	QMainWindow(parent)
{
	this->setWindowTitle("View Layout");
}

SecondaryViewLayoutWindow::~SecondaryViewLayoutWindow()
{
}

void print(QString header, QRect r)
{
	std::cout << header << "  (" << r.left() << ", " << r.top() << ", " << r.width() << ", " << r.height() << ")"<< std::endl;
}

QString SecondaryViewLayoutWindow::toString(QRect r) const
{
	return QString("%1, %2, %3, %4").arg(r.left()).arg(r.top()).arg(r.width()).arg(r.height());
}

void SecondaryViewLayoutWindow::tryShowOnSecondaryScreen()
{
	this->show();

	int bestScreen = this->findSmallestSecondaryScreen();

	QDesktopWidget* desktop = QApplication::desktop();

	if (desktop->primaryScreen()==bestScreen)
	{
		report(QString("No secondary screen found. Displaying secondary view layout on primary screen."));
	}
	else
	{
		QRect rect = desktop->screenGeometry(bestScreen);
		report(QString("Displaying secondary view layout on fullscreen %1 of %2, size=[%3]")
								   .arg(bestScreen+1)
								   .arg(desktop->screenCount())
								   .arg(this->toString(rect)));
		this->setGeometry(rect);
		this->move(rect.topLeft());
		this->setWindowState(this->windowState() | Qt::WindowFullScreen);
	}
}

int SecondaryViewLayoutWindow::findSmallestSecondaryScreen()
{
	QDesktopWidget* desktop = QApplication::desktop();

	int best = 0;
	for (int i=1; i<desktop->screenCount(); ++i)
	{
		if (desktop->primaryScreen()==i)
			continue;
		QRect last = desktop->screenGeometry(best);
		QRect current = desktop->screenGeometry(i);
		if (current.height()*current.width() < last.height()*last.width())
			best = i;
		if (desktop->primaryScreen()==best)
			best = i;
	}

	return best;
}

void SecondaryViewLayoutWindow::showEvent(QShowEvent* event)
{
	this->setCentralWidget(logicManager()->getViewService()->createLayoutWidget(this, 1));
	if (logicManager()->getViewService()->getActiveLayout(1).isEmpty())
		logicManager()->getViewService()->setActiveLayout("LAYOUT_OBLIQUE_3DAnyDual_x1", 1);
}

void SecondaryViewLayoutWindow::hideEvent(QCloseEvent* event)
{
	logicManager()->getViewService()->setActiveLayout("", 1);
}

void SecondaryViewLayoutWindow::closeEvent(QCloseEvent *event)
{
	logicManager()->getViewService()->setActiveLayout("", 1);
}

} // namespace cx

