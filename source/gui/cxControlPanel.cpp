/*
 * cxControlPanel.cpp
 *
 *  Created on: Oct 18, 2010
 *      Author: christiana
 */
#include "cxControlPanel.h"

#include "cxCameraControlWidget.h"


namespace cx
{

ControlPanel::ControlPanel(QWidget* parent) : QMainWindow(parent)
{
  this->setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);

  this->addAsDockWidget(new CameraControlWidget(this));
}

ControlPanel::~ControlPanel()
{

}

void ControlPanel::addAsDockWidget(QWidget* widget)
{
//  if (!mToggleWidgetActionGroup)
//  {
//    mToggleWidgetActionGroup = new QActionGroup(this);
//    mToggleWidgetActionGroup->setExclusive(false);
//  }

  QDockWidget* dockWidget = new QDockWidget(widget->windowTitle(), this);
  dockWidget->setObjectName(widget->objectName() + "DockWidget");
  dockWidget->setWidget(widget);
  this->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
  dockWidget->setVisible(false); // default visibility

//  mToggleWidgetActionGroup->addAction(dockWidget->toggleViewAction());
}



} // namespace cx
