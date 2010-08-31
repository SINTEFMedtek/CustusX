/*
 * cxDataInterface.cpp
 *
 *  Created on: Apr 13, 2010
 *      Author: christiana
 */
#include "cxDataInterface.h"
#include "sscImage.h"
#include "sscMessageManager.h"
#include "cxDataManager.h"
#include "sscImageLUT2D.h"
#include "cxDataManager.h"
#include "sscToolManager.h"
#include "sscTool.h"

namespace cx
{

DoubleDataAdapterActiveToolOffset::DoubleDataAdapterActiveToolOffset()
{
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const std::string&)), this, SLOT(dominantToolChangedSlot()));
  dominantToolChangedSlot();
}

double DoubleDataAdapterActiveToolOffset::getValue() const
{
  if (mTool)
    return mTool->getTooltipOffset();
  return 0.0;
}

bool DoubleDataAdapterActiveToolOffset::setValue(double val)
{
  if (!mTool)
    return false;
  mTool->setTooltipOffset(val);
  return true;
}

void DoubleDataAdapterActiveToolOffset::dominantToolChangedSlot()
{
  if (mTool)
  {
    disconnect(mTool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(changed()));
  }

  mTool = ssc::toolManager()->getDominantTool();

  if (mTool)
  {
    connect(mTool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(changed()));
  }

  emit changed();
}

ssc::DoubleRange DoubleDataAdapterActiveToolOffset::getValueRange() const
{
  double range = 200;
  return ssc::DoubleRange(0,range,range/1000.0);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


DoubleDataAdapterActiveImageBase::DoubleDataAdapterActiveImageBase()
{
  connect(ssc::dataManager(), SIGNAL(activeImageChanged(const std::string&)), this, SLOT(activeImageChanged()));
  connect(ssc::dataManager(), SIGNAL(activeImageTransferFunctionsChanged()), this, SIGNAL(changed()));
}
void DoubleDataAdapterActiveImageBase::activeImageChanged()
{
  mImage = ssc::dataManager()->getActiveImage();
  emit changed();
}
double DoubleDataAdapterActiveImageBase::getValue() const
{
  if (!mImage)
    return 0.0;
  return getValueInternal();
}
bool DoubleDataAdapterActiveImageBase::setValue(double val)
{
  if (!mImage)
    return false;
  setValueInternal(val);
  return true;
}

//---------------------------------------------------------
//---------------------------------------------------------

double DoubleDataAdapter2DWindow::getValueInternal() const
{
  return mImage->getLookupTable2D()->getWindow();
}
void DoubleDataAdapter2DWindow::setValueInternal(double val)
{
  mImage->getLookupTable2D()->setWindow(val);
}
ssc::DoubleRange DoubleDataAdapter2DWindow::getValueRange() const
{
  if (!mImage)
    return ssc::DoubleRange();
  double range = mImage->getRange();
  return ssc::DoubleRange(1,range,range/1000.0);
}




//---------------------------------------------------------
//---------------------------------------------------------




double DoubleDataAdapter2DLevel::getValueInternal() const
{
  return mImage->getLookupTable2D()->getLevel();
}
void DoubleDataAdapter2DLevel::setValueInternal(double val)
{
  mImage->getLookupTable2D()->setLevel(val);
}
ssc::DoubleRange DoubleDataAdapter2DLevel::getValueRange() const
{
  if (!mImage)
    return ssc::DoubleRange();

  double max = mImage->getMax();
  return ssc::DoubleRange(1,max,max/1000.0);
}




//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------




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

} // namespace cx
