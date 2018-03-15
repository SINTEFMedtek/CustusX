/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTransferFunctionWidget.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QStringList>
#include <QInputDialog>
#include <QPushButton>
#include <QActionGroup>
#include <QToolButton>
#include <QAction>
#include <QMessageBox>

#include "cxImageTF3D.h"
#include "cxImageLUT2D.h"

#include "cxTypeConversions.h"
#include "cxTransferFunctionPresetWidget.h"
#include "cxTransferFunctionAlphaWidget.h"
#include "cxTransferFunctionColorWidget.h"
#include "cxShadingWidget.h"
#include "cxDataViewSelectionWidget.h"
#include "cxShadingParamsInterfaces.h"
#include "cxSettings.h"
#include "cxPatientModelService.h"
#include "cxActiveData.h"

namespace cx
{

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

DoublePropertyImageTFDataBase::DoublePropertyImageTFDataBase()
{
}

void DoublePropertyImageTFDataBase::setImageTFData(ImageTFDataPtr tfData, ImagePtr image)
{
  if (mImage)
	  disconnect(mImage.get(), &Image::transferFunctionsChanged, this, &Property::changed);

  mImageTFData = tfData;
  mImage = image;

  if (image)
	  connect(image.get(), &Image::transferFunctionsChanged, this, &Property::changed);

  emit changed();
}

double DoublePropertyImageTFDataBase::getValue() const
{
  if (!mImageTFData)
    return 0.0;
  return this->getValueInternal();
}

bool DoublePropertyImageTFDataBase::setValue(double val)
{
  if (!mImageTFData)
    return false;
  this->setValueInternal(val);
  return true;
}

//---------------------------------------------------------
//---------------------------------------------------------

double DoublePropertyImageTFDataWindow::getValueInternal() const
{
  return mImageTFData->getWindow();
}

void DoublePropertyImageTFDataWindow::setValueInternal(double val)
{
  mImageTFData->setWindow(val);
}

DoubleRange DoublePropertyImageTFDataWindow::getValueRange() const
{
  if (!mImage)
    return DoubleRange();
  double range = mImage->getMax() - mImage->getMin();
  return DoubleRange(1,range,range/1000.0);
}

//---------------------------------------------------------
//---------------------------------------------------------

double DoublePropertyImageTFDataLevel::getValueInternal() const
{
  return mImageTFData->getLevel();
}

void DoublePropertyImageTFDataLevel::setValueInternal(double val)
{
  mImageTFData->setLevel(val);
}

DoubleRange DoublePropertyImageTFDataLevel::getValueRange() const
{
  if (!mImageTFData)
    return DoubleRange();

  double max = mImage->getMax();
  double min = mImage->getMin();
  return DoubleRange(min,max,1);
}

//---------------------------------------------------------
//---------------------------------------------------------

double DoublePropertyImageTFDataLLR::getValueInternal() const
{
  return mImageTFData->getLLR();
}
void DoublePropertyImageTFDataLLR::setValueInternal(double val)
{
  mImageTFData->setLLR(val);
}
DoubleRange DoublePropertyImageTFDataLLR::getValueRange() const
{
  if (!mImageTFData)
    return DoubleRange();

  double max = mImage->getMax();
  double min = mImage->getMin();
	//Set range to min - 1 to allow an llr that shows all values
	return DoubleRange(min - 1,max,(max-min)/1000.0);
}

//---------------------------------------------------------
//---------------------------------------------------------

double DoublePropertyImageTFDataAlpha::getValueInternal() const
{
  return mImageTFData->getAlpha();
}
void DoublePropertyImageTFDataAlpha::setValueInternal(double val)
{
  mImageTFData->setAlpha(val);
}
DoubleRange DoublePropertyImageTFDataAlpha::getValueRange() const
{
  if (!mImageTFData)
    return DoubleRange();

  double max = 1.0;
  return DoubleRange(0,max,max/100.0);
}

//---------------------------------------------------------
//---------------------------------------------------------

TransferFunction3DWidget::TransferFunction3DWidget(ActiveDataPtr activeData, QWidget* parent, bool connectToActiveImage) :
  BaseWidget(parent, "transfer_function_3d_widget", "3D"),
  mLayout(new QVBoxLayout(this)),
  mActiveImageProxy(ActiveImageProxyPtr()),
  mActiveData(activeData)
{
	this->setToolTip("Set a transfer function on a 3D volume");
  mTransferFunctionAlphaWidget = new TransferFunctionAlphaWidget(activeData, this);
  mTransferFunctionColorWidget = new TransferFunctionColorWidget(activeData, this);

  mTransferFunctionAlphaWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                                              QSizePolicy::MinimumExpanding);
  mTransferFunctionColorWidget->setSizePolicy(QSizePolicy::Expanding,
                                              QSizePolicy::Fixed);

  mLayout->addWidget(mTransferFunctionAlphaWidget);
  mLayout->addWidget(mTransferFunctionColorWidget);

  this->setLayout(mLayout);

  if(connectToActiveImage)
  {
	  mActiveImageProxy = ActiveImageProxy::New(mActiveData);
	  connect(mActiveImageProxy.get(), &ActiveImageProxy::activeImageChanged, this, &TransferFunction3DWidget::activeImageChangedSlot);
	  connect(mActiveImageProxy.get(), &ActiveImageProxy::transferFunctionsChanged, this, &TransferFunction3DWidget::activeImageChangedSlot);
  }
  this->activeImageChangedSlot();
}

void TransferFunction3DWidget::activeImageChangedSlot()
{
  ImagePtr activeImage = mActiveData->getActive<Image>();
  this->imageChangedSlot(activeImage);
}

void TransferFunction3DWidget::imageChangedSlot(ImagePtr image)
{
	ImageTFDataPtr tf;
	if (image)
	  tf = image->getTransferFunctions3D();
	else
	  image.reset();

	mTransferFunctionAlphaWidget->setData(image, tf);
	mTransferFunctionColorWidget->setData(image, tf);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

TransferFunction2DWidget::TransferFunction2DWidget(ActiveDataPtr activeData, QWidget* parent, bool connectToActiveImage) :
  BaseWidget(parent, "transfer_function_2d_widget", "2D"),
  mLayout(new QVBoxLayout(this)),
  mActiveData(activeData)
{
	this->setToolTip("Set a transfer function on a 2D image");
  mTransferFunctionAlphaWidget = new TransferFunctionAlphaWidget(activeData, this);
  mTransferFunctionAlphaWidget->setReadOnly(true);
  mTransferFunctionColorWidget = new TransferFunctionColorWidget(activeData, this);

  mDataWindow.reset(new DoublePropertyImageTFDataWindow);
  mDataLevel.reset(new DoublePropertyImageTFDataLevel);
  mDataAlpha.reset(new DoublePropertyImageTFDataAlpha);
  mDataLLR.reset(new DoublePropertyImageTFDataLLR);

  mActiveImageProxy = ActiveImageProxy::New(mActiveData);
  connect(mActiveImageProxy.get(), &ActiveImageProxy::activeImageChanged, this, &TransferFunction2DWidget::activeImageChangedSlot);
  connect(mActiveImageProxy.get(), &ActiveImageProxy::transferFunctionsChanged, this, &TransferFunction2DWidget::activeImageChangedSlot);

  mTransferFunctionAlphaWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                                              QSizePolicy::MinimumExpanding);
  mTransferFunctionColorWidget->setSizePolicy(QSizePolicy::Expanding,
                                              QSizePolicy::Fixed);

  mLayout->addWidget(mTransferFunctionAlphaWidget);
  mLayout->addWidget(mTransferFunctionColorWidget);

  QGridLayout* gridLayout = new QGridLayout;
  mLayout->addLayout(gridLayout);
  new SliderGroupWidget(this, mDataWindow, gridLayout, 0);
  new SliderGroupWidget(this, mDataLevel,  gridLayout, 1);
  new SliderGroupWidget(this, mDataAlpha,  gridLayout, 2);
  new SliderGroupWidget(this, mDataLLR,    gridLayout, 3);

  this->setLayout(mLayout);
  this->activeImageChangedSlot();
}

void TransferFunction2DWidget::activeImageChangedSlot()
{
  ImagePtr image = mActiveData->getActive<Image>();
  ImageTFDataPtr tf;
  if (image)
    tf = image->getLookupTable2D();
  else
    image.reset();

  mTransferFunctionAlphaWidget->setData(image, tf);
  mTransferFunctionColorWidget->setData(image, tf);

  mDataWindow->setImageTFData(tf, image);
  mDataLevel->setImageTFData(tf, image);
  mDataAlpha->setImageTFData(tf, image);
  mDataLLR->setImageTFData(tf, image);
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

TransferFunctionWidget::TransferFunctionWidget(PatientModelServicePtr patientModelService, QWidget* parent, bool connectToActiveImage) :
  BaseWidget(parent, "transfer_function_widget", "Transfer Function")
{
	this->setToolTip("Set a new or predefined transfer function on a volume");
  QVBoxLayout* mLayout = new QVBoxLayout(this);

  TransferFunction3DWidget* transferFunctionWidget = new TransferFunction3DWidget(patientModelService->getActiveData(), this, connectToActiveImage);

  mLayout->setMargin(0);
  mLayout->addWidget(transferFunctionWidget);
  mLayout->addWidget(new TransferFunctionPresetWidget(patientModelService, this, true));

  this->setLayout(mLayout);
}

}//namespace cx
