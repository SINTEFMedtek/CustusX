#ifndef CXTRANSFERFUNCTION2DCOLORWIDGET_H_
#define CXTRANSFERFUNCTION2DCOLORWIDGET_H_

#include "cxBaseWidget.h"
#include "cxTransferFunctionWidget.h"
#include "cxTransferFunctionAlphaWidget.h"
#include "cxTransferFunctionColorWidget.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxGUI
 * @{
 */

/**
 * \class TransferFunction2DColorWidget
 *
 * \date 2010.04.12
 * \author Christian Askeland, SINTEF
 * \author Ole Vegard Solberg, SINTEF
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

/**
 * @}
 */
}//end namespace cx

#endif /* CXTRANSFERFUNCTION2DCOLORWIDGET_H_ */
