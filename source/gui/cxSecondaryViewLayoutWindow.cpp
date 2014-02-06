// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxSecondaryViewLayoutWindow.h"

#include "cxViewManager.h"
#include "sscLogger.h"
#include <QApplication>
#include <QDesktopWidget>
#include "sscTypeConversions.h"
#include "sscMessageManager.h"

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
		messageManager()->sendInfo(QString("No secondary screen found. Displaying secondary view layout on primary screen."));
	}
	else
	{
		QRect rect = desktop->screenGeometry(bestScreen);
		messageManager()->sendInfo(QString("Displaying secondary view layout on fullscreen %1 of %2, size=[%3]")
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
	this->setCentralWidget(viewManager()->getLayoutWidget(1));
	if (viewManager()->getActiveLayout(1).isEmpty())
		viewManager()->setActiveLayout("LAYOUT_OBLIQUE_3DAnyDual_x1", 1);
}

void SecondaryViewLayoutWindow::hideEvent(QCloseEvent* event)
{
	viewManager()->setActiveLayout("", 1);
}

void SecondaryViewLayoutWindow::closeEvent(QCloseEvent *event)
{
	viewManager()->setActiveLayout("", 1);
}

} // namespace cx

