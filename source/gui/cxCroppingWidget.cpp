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

InteractiveCropperPtr mInteractiveCropper;

QCheckBox* mUseCropperCheckBox;
QCheckBox* mShowBoxCheckBox;


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
  mShowBoxCheckBox = new QCheckBox("Show box");
  connect(mShowBoxCheckBox, SIGNAL(toggled(bool)), mInteractiveCropper.get(), SLOT(showBoxWidget(bool)));
  activeLayout->addWidget(mShowBoxCheckBox);

  layout->addStretch();
//  QPushButton* saveButton = new QPushButton("Save clip plane");
//  saveButton->setEnabled(false);
//  QPushButton* clearButton = new QPushButton("Clear saved planes");
//  clearButton->setEnabled(false);
//  activeLayout->addWidget(saveButton);
//  layout->addWidget(clearButton);

  this->cropperChangedSlot();
}

void CroppingWidget::cropperChangedSlot()
{
  mUseCropperCheckBox->setChecked(mInteractiveCropper->getUseCropping());
  mShowBoxCheckBox->setChecked(mInteractiveCropper->getShowBoxWidget());
}

}
