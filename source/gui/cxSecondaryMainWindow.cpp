/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxSecondaryMainWindow.h"

#include "cxTrackPadWidget.h"

namespace cx
{

SecondaryMainWindow::SecondaryMainWindow(QWidget* parent, QWidget* widget) :
	QMainWindow(parent, Qt::WindowStaysOnTopHint)
{
//	this->setWindowTitle("Control Panel");
	this->setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);

	widget->setParent(this);
	this->addAsDockWidget(widget, true);

	//Define geometry to prevent the window from being placed in the center of the screen.
	this->setGeometry(0, 600, 600, 400);
}

SecondaryMainWindow::~SecondaryMainWindow()
{

}

void SecondaryMainWindow::addAsDockWidget(QWidget* widget, bool visible)
{
	QDockWidget* dockWidget = new QDockWidget(widget->windowTitle(), this);
	dockWidget->setObjectName(widget->objectName() + "DockWidget");
	dockWidget->setWidget(widget);
	this->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
	dockWidget->setVisible(visible); // default visibility
}

} // namespace cx
