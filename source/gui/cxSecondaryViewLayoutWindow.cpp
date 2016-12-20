/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

