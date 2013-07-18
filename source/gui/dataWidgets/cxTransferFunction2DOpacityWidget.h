
#ifndef CXTRANSFERFUNCTION2DOPACITYWIDGET_H_
#define CXTRANSFERFUNCTION2DOPACITYWIDGET_H_

#include "cxBaseWidget.h"
#include "cxTransferFunctionWidget.h"
#include "cxTransferFunctionAlphaWidget.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxGUI
 * @{
 */

/**
 * \class TransferFunction2DOpacityWidget
 *
 * \date 2010.04.12
 * \author Christian Askeland, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
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
 * @}
 */
}//end namespace cx

#endif /* CXTRANSFERFUNCTION2DOPACITYWIDGET_H_ */
