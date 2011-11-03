/*
 * cxSecondaryMainWindow.cpp
 *
 *  Created on: Oct 18, 2010
 *      Author: christiana
 */
#include "cxSecondaryMainWindow.h"

#include "cxTrackPadWidget.h"

namespace cx
{

SecondaryMainWindow::SecondaryMainWindow(QWidget* parent) :
	QMainWindow(parent)
{
	this->setWindowTitle("Control Panel");
	this->setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);

	this->addAsDockWidget(new TrackPadWidget(this), true);
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
