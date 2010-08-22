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
#include "cxShadingWidget.h"

namespace cx
{

ActiveImageStringDataAdapter::ActiveImageStringDataAdapter()
{
  connect(ssc::dataManager(), SIGNAL(dataLoaded()),                         this, SIGNAL(changed()));
  connect(ssc::dataManager(), SIGNAL(currentImageDeleted(ssc::ImagePtr)),   this, SIGNAL(changed()));
  connect(ssc::dataManager(), SIGNAL(activeImageChanged(std::string)),      this, SIGNAL(changed()));
}

QString ActiveImageStringDataAdapter::getValueName() const
{
  return "Active Volume";
}
bool ActiveImageStringDataAdapter::setValue(const QString& value)
{
  ssc::ImagePtr newImage = ssc::dataManager()->getImage(string_cast(value));
  if (newImage==ssc::dataManager()->getActiveImage())
    return false;
  ssc::dataManager()->setActiveImage(newImage);
  return true;
}
QString ActiveImageStringDataAdapter::getValue() const
{
  if (!ssc::dataManager()->getActiveImage())
    return "";
  return qstring_cast(ssc::dataManager()->getActiveImage()->getUid());
}
QString ActiveImageStringDataAdapter::getHelp() const
{
  return "select the active volume";
}
QStringList ActiveImageStringDataAdapter::getValueRange() const
{
  std::vector<std::string> uids = ssc::dataManager()->getImageUids();
  QStringList retval;
  retval << "";
  for (unsigned i=0; i<uids.size(); ++i)
    retval << qstring_cast(uids[i]);
  return retval;
}
QString ActiveImageStringDataAdapter::convertInternal2Display(QString internal)
{
  ssc::ImagePtr image = ssc::dataManager()->getImage(string_cast(internal));
  if (!image)
    return "<no volume>";
  return qstring_cast(image->getName());
}

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
  tabWidget->addTab(new CroppingWidget(this), "Cropping");
}

}
