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
//QString RegistrationFixedImageStringDataAdapter::getValueName() const
//{
//  return "Fixed Volume";
//}

bool RegistrationFixedImageStringDataAdapter::setValue(const QString& value)
{
//  std::cout << "RegistrationFixedImageStringDataAdapter::setImageSlot " << value << std::endl;

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

//QString RegistrationFixedImageStringDataAdapter::getHelp() const
//{
//  return "Select the fixed registration data";
//}

//DataPtr RegistrationFixedImageStringDataAdapter::getData() const
//{
//    return mManager->getFixedData();
//}

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
//QString RegistrationMovingImageStringDataAdapter::getValueName() const
//{
//  return "Moving Volume";
//}

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

//QString RegistrationMovingImageStringDataAdapter::getHelp() const
//{
//  return "Select the moving registration data";
//}

//DataPtr RegistrationMovingImageStringDataAdapter::getData() const
//{
//    return mManager->getMovingData();
//}


}
