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

void SecondaryViewLayoutWindow::tryShowOnSecondaryScreen()
{
	QDesktopWidget* desktop = QApplication::desktop();
	SSC_ASSERT(desktop);
	print(QString("def screen:"), desktop->screenGeometry());
	print(QString("screen 0:"), desktop->screenGeometry(0));

	this->show();

	if (desktop->screenCount()>1)
	{
		print(QString("screen 1:"), desktop->screenGeometry(1));
		int bestScreen = 1;
		for (int i=2; i<desktop->screenCount(); ++i)
		{
			print(QString("screen %1:").arg(i), desktop->screenGeometry(i));
			QRect last = desktop->screenGeometry(bestScreen);
			QRect current = desktop->screenGeometry(i);
			if (current.height()*current.width() < last.height()*last.width())
				bestScreen = i;
		}

		std::cout << "Displaying secondary view layout on screen " << bestScreen << std::endl;
		QRect rect = desktop->screenGeometry(bestScreen);
		print(QString("using rect:"), rect);
		this->setGeometry(rect);

//		QRect rect = desktop->screenGeometry(1);
//		move(rect.topLeft());
//		setWindowState(Qt::WindowFullScreen);

		//mSecondaryViewLayoutWindow->setWindowState(mSecondaryViewLayoutWindow->windowState() | Qt::WindowFullScreen);
	}
}

void SecondaryViewLayoutWindow::showEvent(QShowEvent* event)
{
	viewManager()->setActiveLayout("LAYOUT_3D", 1);
//	viewManager()->setActiveLayout("LAYOUT_OBLIQUE_3DAnyDual_x1", 1);
	this->setCentralWidget(viewManager()->getLayoutWidget(1));
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

