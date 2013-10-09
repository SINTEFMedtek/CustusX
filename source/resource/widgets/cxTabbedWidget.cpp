#include "cxTabbedWidget.h"

#include <iostream>
#include <QTabWidget>
#include <QVBoxLayout>

////segmentation
//#include "cxConnectedThresholdImageFilterWidget.h"
//#include "cxBinaryThresholdImageFilterWidget.h"
//
//#include "cxSurfaceWidget.h"
//////registration
////#include "cxImageRegistrationWidget.h"
////#include "cxPatientRegistrationWidget.h"
////#include "cxFastImageRegistrationWidget.h"
////#include "cxFastPatientRegistrationWidget.h"
////#include "cxFastOrientationRegistrationWidget.h"
////#include "cxImageSegmentationAndCenterlineWidget.h"
////#include "cxPlateRegistrationWidget.h"
////#include "cxManualRegistrationOffsetWidget.h"
////#include "cxRegisterI2IWidget.h"
//#include <cxToolTipSampleWidget.h>
////calibration
//#include "cxToolTipCalibrationWidget.h"

namespace cx
{
//------------------------------------------------------------------------------
TabbedWidget::TabbedWidget(QWidget* parent, QString objectName, QString windowTitle) :
    BaseWidget(parent, objectName, windowTitle),
    mTabWidget(new QTabWidget(this))
{
	mTabWidget->setElideMode(Qt::ElideRight);
	mLayout = new QVBoxLayout(this);
	mLayout->addWidget(mTabWidget);
	mLayout->setMargin(2); // lots of tabbed widgets in layers use up the desktop. Must reduce.
}

TabbedWidget::~TabbedWidget()
{}

void TabbedWidget::addTab(BaseWidget* newTab, QString newTabName)
{
  int index = mTabWidget->addTab(newTab, newTabName);
  mTabWidget->setTabToolTip(index, newTab->defaultWhatsThis());
  mTabWidget->setTabWhatsThis(index, newTab->defaultWhatsThis());
  newTab->setWhatsThis(newTab->defaultWhatsThis());
}

void TabbedWidget::insertWidgetAtTop(QWidget* newWidget)
{
	mLayout->insertWidget(0, newWidget);
}

QString TabbedWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Tabbed widget.</h3>"
      "<p>This is a tabbed widget, used for creating a hierarchy of tabbed widgets.</p>"
      "<p><i>If you see this whats this message, something's wrong!</i></p>"
      "</html>";
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
} //namespace cx
