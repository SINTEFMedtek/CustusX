/*
 * cxDataInterface.cpp
 *
 *  Created on: Apr 13, 2010
 *      Author: christiana
 */
#include "cxDataInterface.h"
#include "sscImage.h"
#include "sscMessageManager.h"
#include "sscImageLUT2D.h"
#include "sscDataManager.h"
#include "sscToolManager.h"
#include "sscTool.h"
#include "sscTypeConversions.h"

namespace cx
{

DoubleDataAdapterActiveToolOffset::DoubleDataAdapterActiveToolOffset()
{
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));
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
  connect(ssc::dataManager(), SIGNAL(activeImageChanged(const QString&)), this, SLOT(activeImageChanged()));
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

SelectImageStringDataAdapterBase::SelectImageStringDataAdapterBase()
{
  connect(ssc::dataManager(), SIGNAL(dataLoaded()),                         this, SIGNAL(changed()));
  connect(ssc::dataManager(), SIGNAL(currentImageDeleted(ssc::ImagePtr)),   this, SIGNAL(changed()));
}
QStringList SelectImageStringDataAdapterBase::getValueRange() const
{
  std::vector<QString> uids = ssc::dataManager()->getImageUids();
  QStringList retval;
  retval << "";
  for (unsigned i=0; i<uids.size(); ++i)
    retval << qstring_cast(uids[i]);
  return retval;
}
QString SelectImageStringDataAdapterBase::convertInternal2Display(QString internal)
{
  ssc::ImagePtr image = ssc::dataManager()->getImage(internal);
  if (!image)
    return "<no volume>";
  return qstring_cast(image->getName());
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

ActiveImageStringDataAdapter::ActiveImageStringDataAdapter()
{
  connect(ssc::dataManager(), SIGNAL(activeImageChanged(QString)),      this, SIGNAL(changed()));
}
QString ActiveImageStringDataAdapter::getValueName() const
{
  return "Active Volume";
}
bool ActiveImageStringDataAdapter::setValue(const QString& value)
{
  ssc::ImagePtr newImage = ssc::dataManager()->getImage(value);
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
  return "Select the active volume";
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

RegistrationFixedImageStringDataAdapter::RegistrationFixedImageStringDataAdapter()
{
  connect(registrationManager(), SIGNAL(fixedDataChanged(QString)), this, SIGNAL(changed()));
}
QString RegistrationFixedImageStringDataAdapter::getValueName() const
{
  return "Fixed Volume";
}

bool RegistrationFixedImageStringDataAdapter::setValue(const QString& value)
{
  ssc::ImagePtr newImage = ssc::dataManager()->getImage(value);
  if (newImage==registrationManager()->getFixedData())
    return false;
  registrationManager()->setFixedData(newImage);
  return true;
}
QString RegistrationFixedImageStringDataAdapter::getValue() const
{
  ssc::ImagePtr image = boost::dynamic_pointer_cast<ssc::Image>(registrationManager()->getFixedData());
  if (!image)
    return "";
  return qstring_cast(image->getUid());
}
QString RegistrationFixedImageStringDataAdapter::getHelp() const
{
  return "Select the fixed registration volume";
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

RegistrationMovingImageStringDataAdapter::RegistrationMovingImageStringDataAdapter()
{
  connect(registrationManager(), SIGNAL(movingDataChanged(QString)), this, SIGNAL(changed()));
}
QString RegistrationMovingImageStringDataAdapter::getValueName() const
{
  return "Moving Volume";
}
bool RegistrationMovingImageStringDataAdapter::setValue(const QString& value)
{
  ssc::ImagePtr newImage = ssc::dataManager()->getImage(value);
  if (newImage==registrationManager()->getMovingData())
    return false;
  registrationManager()->setMovingData(newImage);
  return true;
}
QString RegistrationMovingImageStringDataAdapter::getValue() const
{
  ssc::ImagePtr image = boost::dynamic_pointer_cast<ssc::Image>(registrationManager()->getMovingData());
  if (!image)
    return "";
  return qstring_cast(image->getUid());
}
QString RegistrationMovingImageStringDataAdapter::getHelp() const
{
  return "Select the moving registration volume";
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

SelectImageStringDataAdapter::SelectImageStringDataAdapter()
{
}
QString SelectImageStringDataAdapter::getValueName() const
{
  return "Select Volume";
}
bool SelectImageStringDataAdapter::setValue(const QString& value)
{
  if (value==mImageUid)
    return false;
  mImageUid = value;
  emit changed();
  emit imageChanged(mImageUid);
  return true;
}
QString SelectImageStringDataAdapter::getValue() const
{
  return mImageUid;
}
QString SelectImageStringDataAdapter::getHelp() const
{
  return "Select a volume";
}
ssc::ImagePtr SelectImageStringDataAdapter::getImage()
{
  return ssc::dataManager()->getImage(mImageUid);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------




ParentFrameStringDataAdapter::ParentFrameStringDataAdapter()
{
  connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SIGNAL(changed()));
}

void ParentFrameStringDataAdapter::setData(ssc::DataPtr data)
{
  mData = data;
  connect(mData.get(), SIGNAL(transformChanged()), this, SIGNAL(changed()));
  emit changed();
}

QString ParentFrameStringDataAdapter::getValueName() const
{
  return "Parent Frame";
}

bool ParentFrameStringDataAdapter::setValue(const QString& value)
{
  if (!mData)
    return false;
  mData->setParentFrame(value);
  return true;
}

QString ParentFrameStringDataAdapter::getValue() const
{
  if (!mData)
    return "";
  return qstring_cast(mData->getParentFrame());
}

QString ParentFrameStringDataAdapter::getHelp() const
{
  if (!mData)
    return "";
  return "Select the parent frame for " + qstring_cast(mData->getName()) + ".";
}

QStringList ParentFrameStringDataAdapter::getValueRange() const
{
  QStringList retval;
  retval << "";

  std::map<QString, ssc::DataPtr> allData = ssc::dataManager()->getData();
  for (std::map<QString, ssc::DataPtr>::iterator iter=allData.begin(); iter!=allData.end(); ++iter)
  {
    if (mData && (mData->getUid() == iter->first))
      continue;

    retval << qstring_cast(iter->first);
  }
  return retval;
}

QString ParentFrameStringDataAdapter::convertInternal2Display(QString internal)
{
  ssc::DataPtr data = ssc::dataManager()->getData(internal);
  if (!data)
    return "<no data>";
  return qstring_cast(data->getName());
}


} // namespace cx
