/*
 * cxRegistrationDataAdapters.cpp
 *
 *  \date Jun 16, 2011
 *      \author christiana
 */

#include "cxRegistrationDataAdapters.h"
#include "cxRegistrationManager.h"
#include "cxDataManager.h"
#include "cxTypeConversions.h"

namespace cx
{


RegistrationFixedImageStringDataAdapter::RegistrationFixedImageStringDataAdapter(RegistrationManagerPtr regManager) :
		mManager(regManager)
{
  mValueName = "Fixed Volume";
  mHelp = "Select the fixed registration data";
  connect(mManager.get(), SIGNAL(fixedDataChanged(QString)), this, SIGNAL(changed()));
}

bool RegistrationFixedImageStringDataAdapter::setValue(const QString& value)
{
  DataPtr newImage = dataManager()->getData(value);
  if (newImage==mManager->getFixedData())
    return false;
  mManager->setFixedData(newImage);
  return true;
}
QString RegistrationFixedImageStringDataAdapter::getValue() const
{
  DataPtr image = mManager->getFixedData();
  if (!image)
    return "";
  return qstring_cast(image->getUid());
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

RegistrationMovingImageStringDataAdapter::RegistrationMovingImageStringDataAdapter(RegistrationManagerPtr regManager) :
		mManager(regManager)
{
    mValueName = "Moving Volume";
    mHelp = "Select the moving registration data";
  connect(mManager.get(), SIGNAL(movingDataChanged(QString)), this, SIGNAL(changed()));
}

bool RegistrationMovingImageStringDataAdapter::setValue(const QString& value)
{
  DataPtr newImage = dataManager()->getData(value);
  if (newImage==mManager->getMovingData())
    return false;
  mManager->setMovingData(newImage);
  return true;
}

QString RegistrationMovingImageStringDataAdapter::getValue() const
{
  DataPtr image = mManager->getMovingData();
  if (!image)
    return "";
  return qstring_cast(image->getUid());
}

}
