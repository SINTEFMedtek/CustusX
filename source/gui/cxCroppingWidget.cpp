/*
 * cxCroppingWidget.cpp
 *
 *  Created on: Aug 20, 2010
 *      Author: christiana
 */

#include "cxCroppingWidget.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>
#include "sscStringDataAdapter.h"
#include "sscStringWidgets.h"
#include "sscDefinitionStrings.h"
#include "cxInteractiveCropper.h"

namespace cx
{

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------



CroppingWidget::CroppingWidget(QWidget* parent) : QWidget(parent)
{
  mInteractiveCropper = viewManager()->getCropper();
  connect(mInteractiveCropper.get(), SIGNAL(changed()), this, SLOT(cropperChangedSlot()));

  this->setObjectName("ClippingWidget");
  this->setWindowTitle("Crop");

  QVBoxLayout* layout = new QVBoxLayout(this);

  QGroupBox* activeGroupBox = new QGroupBox("Interactive cropper");
  layout->addWidget(activeGroupBox);
  QVBoxLayout* activeLayout = new QVBoxLayout(activeGroupBox);

//  mPlaneAdapter = ClipPlaneStringDataAdapter::New(mInteractiveClipper);
//  ssc::ComboGroupWidget* combo = new ssc::ComboGroupWidget(this, mPlaneAdapter);

  mUseCropperCheckBox = new QCheckBox("Use Cropper");
  connect(mUseCropperCheckBox, SIGNAL(toggled(bool)), mInteractiveCropper.get(), SLOT(useCropping(bool)));
  activeLayout->addWidget(mUseCropperCheckBox);
  //activeLayout->addWidget(combo);
  mShowBoxCheckBox = new QCheckBox("Show box (i)");
  mShowBoxCheckBox->setToolTip("Show crop box in 3D view. Press 'i' in the view to do the same.");
  connect(mShowBoxCheckBox, SIGNAL(toggled(bool)), mInteractiveCropper.get(), SLOT(showBoxWidget(bool)));
  activeLayout->addWidget(mShowBoxCheckBox);

  mXRange = new SliderRangeGroupWidget(this);
  mXRange->setName("X");
  mXRange->setRange(ssc::DoubleRange(-2000, 2000, 1));
  connect(mXRange, SIGNAL(valueChanged(double,double)), this, SLOT(boxValuesChanged()));
  layout->addWidget(mXRange);

  mYRange = new SliderRangeGroupWidget(this);
  mYRange->setName("Y");
  mYRange->setRange(ssc::DoubleRange(-2000, 2000, 1));
  connect(mYRange, SIGNAL(valueChanged(double,double)), this, SLOT(boxValuesChanged()));
  layout->addWidget(mYRange);

  mZRange = new SliderRangeGroupWidget(this);
  mZRange->setName("Z");
  mZRange->setRange(ssc::DoubleRange(-2000, 2000, 1));
  connect(mZRange, SIGNAL(valueChanged(double,double)), this, SLOT(boxValuesChanged()));
  layout->addWidget(mZRange);

  QxtSpanSlider* spanSlider = new QxtSpanSlider(this);
  spanSlider->setOrientation(Qt::Horizontal);
  spanSlider->setRange(-500, 500);
  spanSlider->setLowerValue(-200);
  spanSlider->setUpperValue(200);
  spanSlider->setHandleMovementMode(QxtSpanSlider::NoOverlapping);
  layout->addWidget(spanSlider);

  layout->addStretch();
//  QPushButton* saveButton = new QPushButton("Save clip plane");
//  saveButton->setEnabled(false);
//  QPushButton* clearButton = new QPushButton("Clear saved planes");
//  clearButton->setEnabled(false);
//  activeLayout->addWidget(saveButton);
//  layout->addWidget(clearButton);

  this->cropperChangedSlot();
}

void CroppingWidget::boxValuesChanged()
{
  std::pair<double,double> x = mXRange->getValue();
  std::pair<double,double> y = mYRange->getValue();
  std::pair<double,double> z = mZRange->getValue();
  ssc::DoubleBoundingBox3D box(x.first, x.second, y.first, y.second, z.first, z.second);
  mInteractiveCropper->setBoundingBox(box);
}

void CroppingWidget::cropperChangedSlot()
{
  mUseCropperCheckBox->setChecked(mInteractiveCropper->getUseCropping());
  mShowBoxCheckBox->setChecked(mInteractiveCropper->getShowBoxWidget());

  ssc::DoubleBoundingBox3D range =  mInteractiveCropper->getMaxBoundingBox();
//std::cout << "CroppingWidget::cropperChangedSlot(" << box << ")" << std::endl;
  mXRange->setRange(ssc::DoubleRange(range.begin()[0], range.begin()[1], 1));
  mYRange->setRange(ssc::DoubleRange(range.begin()[2], range.begin()[3], 1));
  mZRange->setRange(ssc::DoubleRange(range.begin()[4], range.begin()[5], 1));

  ssc::DoubleBoundingBox3D box =  mInteractiveCropper->getBoundingBox();
//std::cout << "CroppingWidget::cropperChangedSlot(" << box << ")" << std::endl;
  mXRange->setValue(std::make_pair(box.begin()[0], box.begin()[1]));
  mYRange->setValue(std::make_pair(box.begin()[2], box.begin()[3]));
  mZRange->setValue(std::make_pair(box.begin()[4], box.begin()[5]));
}

}
