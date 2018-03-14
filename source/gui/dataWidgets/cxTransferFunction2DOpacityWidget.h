/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTRANSFERFUNCTION2DOPACITYWIDGET_H_
#define CXTRANSFERFUNCTION2DOPACITYWIDGET_H_

#include "cxGuiExport.h"

#include "cxBaseWidget.h"
#include "cxTransferFunctionWidget.h"
#include "cxTransferFunctionAlphaWidget.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_gui
 * @{
 */

/**
 * \class TransferFunction2DOpacityWidget
 *
 * \date 2010.04.12
 * \author Christian Askeland, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class cxGui_EXPORT TransferFunction2DOpacityWidget : public BaseWidget
{
  Q_OBJECT

public:
  TransferFunction2DOpacityWidget(ActiveDataPtr activeData, QWidget* parent);
  virtual ~TransferFunction2DOpacityWidget();

private slots:
  void activeImageChangedSlot();

protected:
  TransferFunctionAlphaWidget* mTransferFunctionAlphaWidget;
  DoublePropertyImageTFDataBasePtr mDataAlpha, mDataLLR;
  ActiveImageProxyPtr mActiveImageProxy;
  ActiveDataPtr mActiveData;
};

/**
 * @}
 */
}//end namespace cx

#endif /* CXTRANSFERFUNCTION2DOPACITYWIDGET_H_ */
