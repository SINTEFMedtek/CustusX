#ifndef CXIMAGEPROPERTIESWIDGET_H_
#define CXIMAGEPROPERTIESWIDGET_H_

#include <vector>
#include <QtGui>
#include "sscDoubleWidgets.h"
#include "cxTransferFunctionWidget.h"

namespace cx
{

/*
 *
 */
class TransferFunction2DColorWidget : public QWidget
{
  Q_OBJECT

public:
  TransferFunction2DColorWidget(QWidget* parent);
  virtual ~TransferFunction2DColorWidget() {}

public slots:
  void activeImageChangedSlot();

protected:
  TransferFunctionAlphaWidget* mTransferFunctionAlphaWidget;
  TransferFunctionColorWidget* mTransferFunctionColorWidget;
  DoubleDataAdapterImageTFDataBasePtr mDataWindow, mDataLevel;
};

/*
 *
 */
class TransferFunction2DOpacityWidget : public QWidget
{
  Q_OBJECT

public:
  TransferFunction2DOpacityWidget(QWidget* parent);
  virtual ~TransferFunction2DOpacityWidget() {}

public slots:
  void activeImageChangedSlot();

protected:
  TransferFunctionAlphaWidget* mTransferFunctionAlphaWidget;
  DoubleDataAdapterImageTFDataBasePtr mDataAlpha, mDataLLR;
};


/**
 * \class ImagePropertiesWidget
 *
 * \date 2010.04.12
 * \author: Christian Askeland, SINTEF
 */
class ImagePropertiesWidget : public QWidget
{
  Q_OBJECT

public:
  ImagePropertiesWidget(QWidget* parent);
  virtual ~ImagePropertiesWidget();

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private:
  ImagePropertiesWidget();

//  TransferFunction2DWidget* mTF2DWidget;
//  TransferFunctionPresetWidget* mTFPresetWidget;

//  ssc::SliderGroupWidget* mLevelWidget;
//  ssc::SliderGroupWidget* mWindowWidget;
};

}//end namespace cx

#endif /* CXIMAGEPROPERTIESWIDGET_H_ */
