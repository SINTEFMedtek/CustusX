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

namespace cx
{


DoubleDataInterfaceWindow::DoubleDataInterfaceWindow()
{
  connect(dataManager(), SIGNAL(activeImageChanged(const std::string&)), this, SIGNAL(valueChanged()));
}
DoubleDataInterfaceWindow::~DoubleDataInterfaceWindow() {}
double DoubleDataInterfaceWindow::getValue() const
{
  ssc::ImagePtr image = dataManager()->getActiveImage();
  if (image)
  {
    return image->getLookupTable2D()->getWindow();
  }
  return 0.0;
}
void DoubleDataInterfaceWindow::setValue(double val)
{
  ssc::ImagePtr image = dataManager()->getActiveImage();
  if (image)
  {
    image->getLookupTable2D()->setWindow(val);
  }
}
QString DoubleDataInterfaceWindow::getName() const
{
  return "Window";
}


DoubleDataInterfaceLevel::DoubleDataInterfaceLevel()
{
  connect(dataManager(), SIGNAL(activeImageChanged(const std::string&)), this, SIGNAL(valueChanged()));
}

DoubleDataInterfaceLevel::~DoubleDataInterfaceLevel()
{
}

double DoubleDataInterfaceLevel::getValue() const
{
  ssc::ImagePtr image = dataManager()->getActiveImage();
  if (image)
  {
    return image->getLookupTable2D()->getLevel();
  }
  return 0.0;
}

void DoubleDataInterfaceLevel::setValue(double val)
{
  ssc::ImagePtr image = dataManager()->getActiveImage();
  if (image)
  {
    image->getLookupTable2D()->setLevel(val);
  }
}

QString DoubleDataInterfaceLevel::getName() const
{
  return "Level";
}


SliderGroup::SliderGroup(QWidget* parent, DoubleDataInterfacePtr dataInterface, QGridLayout* gridLayout, int row)
{
  mData = dataInterface;
  connect(mData.get(), SIGNAL(valueChanged()), this, SLOT(dataChanged()));

  QHBoxLayout* topLayout = new QHBoxLayout;
  topLayout->setMargin(0);
  this->setLayout(topLayout);

  mLabel = new QLabel(this);
  mLabel->setText(mData->getName());
  topLayout->addWidget(mLabel);
  mEdit = new QLineEdit(this);
  topLayout->addWidget(mEdit);
  mSlider = new QSlider(this);
  mSlider->setOrientation(Qt::Horizontal);
  topLayout->addWidget(mSlider);

  connect(mEdit, SIGNAL(textEdited(const QString&)), this, SLOT(textEditedSlot(const QString&)));

  if (gridLayout) // add to input gridlayout
  {
    gridLayout->addWidget(mLabel,  row, 0);
    gridLayout->addWidget(mEdit,   row, 1);
    gridLayout->addWidget(mSlider, row, 2);
  }
  else // add directly to this
  {
    topLayout->addWidget(mLabel);
    topLayout->addWidget(mEdit);
    topLayout->addWidget(mSlider);
  }

  dataChanged();
}

void SliderGroup::textEditedSlot(const QString& text)
{
  bool ok;
  double newVal = text.toDouble(&ok);
  if (!ok)
    return;

  if (ssc::similar(newVal, mData->getValue()))
      return;

  mData->setValue(newVal);
}

void SliderGroup::dataChanged()
{
  mEdit->setText(qstring_cast(mData->getValue()));
}

} // namespace cx
