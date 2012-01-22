#ifndef CXIMAGEPROPERTIESWIDGET_H_
#define CXIMAGEPROPERTIESWIDGET_H_

#include "cxBaseWidget.h"

#include <vector>
#include <QtGui>
#include "sscDoubleWidgets.h"
#include "cxTransferFunctionWidget.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxGUI
 * @{
 */

class TransferFunction2DColorWidget : public BaseWidget
{
  Q_OBJECT

public:
  TransferFunction2DColorWidget(QWidget* parent);
  virtual ~TransferFunction2DColorWidget() {}
  virtual QString defaultWhatsThis() const;

public slots:
  void activeImageChangedSlot();

protected:
  TransferFunctionAlphaWidget* mTransferFunctionAlphaWidget;
  TransferFunctionColorWidget* mTransferFunctionColorWidget;
  DoubleDataAdapterImageTFDataBasePtr mDataWindow, mDataLevel;
  ActiveImageProxyPtr mActiveImageProxy;
};

class TransferFunction2DOpacityWidget : public BaseWidget
{
  Q_OBJECT

public:
  TransferFunction2DOpacityWidget(QWidget* parent);
  virtual ~TransferFunction2DOpacityWidget() {}
  virtual QString defaultWhatsThis() const;

public slots:
  void activeImageChangedSlot();

protected:
  TransferFunctionAlphaWidget* mTransferFunctionAlphaWidget;
  DoubleDataAdapterImageTFDataBasePtr mDataAlpha, mDataLLR;
  ActiveImageProxyPtr mActiveImageProxy;
};


/**
 * \class ImagePropertiesWidget
 *
 * \date 2010.04.12
 * \author: Christian Askeland, SINTEF
 */
class ImagePropertiesWidget : public BaseWidget
{
  Q_OBJECT

public:
  ImagePropertiesWidget(QWidget* parent);
  virtual ~ImagePropertiesWidget();
  virtual QString defaultWhatsThis() const;

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private:
  ImagePropertiesWidget();
};

/**
 * @}
 */
}//end namespace cx

#endif /* CXIMAGEPROPERTIESWIDGET_H_ */
