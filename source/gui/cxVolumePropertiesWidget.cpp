/*
 * cxVolumePropertiesWidget.cpp
 *
 *  Created on: Aug 20, 2010
 *      Author: christiana
 */
#include "cxVolumePropertiesWidget.h"


#include <QComboBox>
#include <QTabWidget>
#include <QVBoxLayout>
//#include <QComboBox>
//#include "sscProbeRep.h"
#include "sscMessageManager.h"
#include "cxDataManager.h"
//#include "cxRegistrationManager.h"
//#include "sscToolManager.h"
#include "cxViewManager.h"
//#include "cxRepManager.h"
//#include "cxView3D.h"
//#include "cxView2D.h"
#include "sscStringWidgets.h"

#include "cxTransferFunctionWidget.h"
#include "cxCroppingWidget.h"
#include "cxClippingWidget.h"
#include "cxShadingWidget.h"
#include "cxDataInterface.h"

namespace cx
{


/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------

ActiveVolumeWidget::ActiveVolumeWidget(QWidget* parent) :
  QWidget(parent)
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  this->setObjectName("ActiveVolumeWidget");
  layout->setMargin(0);

  ssc::ComboGroupWidget*  combo = new ssc::ComboGroupWidget(this, ActiveImageStringDataAdapter::New());
  layout->addWidget(combo);
}

/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------


VolumePropertiesWidget::VolumePropertiesWidget(QWidget* parent) : QWidget(parent)
{
  this->setObjectName("VolumePropertiesWidget");
  this->setWindowTitle("Volume Properties");

  QVBoxLayout* layout = new QVBoxLayout(this);

  layout->addWidget(new ActiveVolumeWidget(this));

  QTabWidget* tabWidget = new QTabWidget(this);
  layout->addWidget(tabWidget);
  tabWidget->addTab(new TransferFunctionWidget(this), "Transfer Functions");
  tabWidget->addTab(new ShadingWidget(this), "Shading");
  tabWidget->addTab(new CroppingWidget(this), "Crop");
  tabWidget->addTab(new ClippingWidget(this), "Clip");
}

}
