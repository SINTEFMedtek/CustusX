/*
 * cxDataInterface.cpp
 *
 *  Created on: Apr 13, 2010
 *      Author: christiana
 */
#include "cxDataInterface.h"
#include "sscImage.h"
#include "sscTypeConversions.h"
#include "cxDataManager.h"
#include "cxMessageManager.h"
#include "sscImageLUT2D.h"
#include "cxDataManager.h"
#include "cxToolManager.h"
#include "sscTool.h"

namespace cx
{

DoubleDataInterfaceActiveToolOffset::DoubleDataInterfaceActiveToolOffset()
{
  connect(toolManager(), SIGNAL(dominantToolChanged(const std::string&)), this, SLOT(dominantToolChangedSlot()));
  dominantToolChangedSlot();
}

double DoubleDataInterfaceActiveToolOffset::getValue() const
{
  if (mTool)
    return mTool->getTooltipOffset();
  return 0.0;
}

bool DoubleDataInterfaceActiveToolOffset::setValue(double val)
{
  if (!mTool)
    return false;
  mTool->setTooltipOffset(val);
  return true;
}

void DoubleDataInterfaceActiveToolOffset::dominantToolChangedSlot()
{
  if (mTool)
  {
    disconnect(mTool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(changed()));
  }

  mTool = toolManager()->getDominantTool();

  if (mTool)
  {
    connect(mTool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(changed()));
  }
}

ssc::DoubleRange DoubleDataInterfaceActiveToolOffset::getValueRange() const
{
  double range = 200;
  return ssc::DoubleRange(0,range,range/1000.0);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


DoubleDataInterfaceActiveImageBase::DoubleDataInterfaceActiveImageBase()
{
  connect(dataManager(), SIGNAL(activeImageChanged(const std::string&)), this, SLOT(activeImageChanged()));
}
void DoubleDataInterfaceActiveImageBase::activeImageChanged()
{
  if (mImage)
    disconnect(mImage->getLookupTable2D().get(), SIGNAL(transferFunctionsChanged()), this, SIGNAL(changed()));

  mImage = dataManager()->getActiveImage();

  if (mImage)
    connect(mImage->getLookupTable2D().get(), SIGNAL(transferFunctionsChanged()), this, SIGNAL(changed()));

  emit changed();
}
double DoubleDataInterfaceActiveImageBase::getValue() const
{
  if (!mImage)
    return 0.0;
  return getValueInternal();
}
bool DoubleDataInterfaceActiveImageBase::setValue(double val)
{
  if (!mImage)
    return false;
  setValueInternal(val);
  return true;
}

//---------------------------------------------------------
//---------------------------------------------------------

double DoubleDataInterface2DWindow::getValueInternal() const
{
  return mImage->getLookupTable2D()->getWindow();
}
void DoubleDataInterface2DWindow::setValueInternal(double val)
{
  mImage->getLookupTable2D()->setWindow(val);
}
ssc::DoubleRange DoubleDataInterface2DWindow::getValueRange() const
{
  if (!mImage)
    return ssc::DoubleRange();
  double range = mImage->getRange();
  return ssc::DoubleRange(1,range,range/1000.0);
}

//---------------------------------------------------------
//---------------------------------------------------------


double DoubleDataInterface2DLevel::getValueInternal() const
{
  return mImage->getLookupTable2D()->getLevel();
}
void DoubleDataInterface2DLevel::setValueInternal(double val)
{
  mImage->getLookupTable2D()->setLevel(val);
}
ssc::DoubleRange DoubleDataInterface2DLevel::getValueRange() const
{
  if (!mImage)
    return ssc::DoubleRange();

  double max = mImage->getMax();
  return ssc::DoubleRange(1,max,max/1000.0);
}


} // namespace cx
