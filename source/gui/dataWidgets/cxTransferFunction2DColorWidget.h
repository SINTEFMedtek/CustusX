/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRANSFERFUNCTION2DCOLORWIDGET_H_
#define CXTRANSFERFUNCTION2DCOLORWIDGET_H_

#include "cxGuiExport.h"

#include "cxBaseWidget.h"
#include "cxTransferFunctionWidget.h"
#include "cxTransferFunctionAlphaWidget.h"
#include "cxTransferFunctionColorWidget.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_gui
 * @{
 */

/**
 * \class TransferFunction2DColorWidget
 *
 * \date 2010.04.12
 * \author Christian Askeland, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class cxGui_EXPORT TransferFunction2DColorWidget : public BaseWidget
{
  Q_OBJECT

public:
  TransferFunction2DColorWidget(ActiveDataPtr activeData, QWidget* parent);
  virtual ~TransferFunction2DColorWidget();

public slots:
  void activeImageChangedSlot();

protected:
  TransferFunctionAlphaWidget* mTransferFunctionAlphaWidget;
  TransferFunctionColorWidget* mTransferFunctionColorWidget;
  DoublePropertyImageTFDataBasePtr mDataWindow, mDataLevel;
  ActiveImageProxyPtr mActiveImageProxy;
  ActiveDataPtr mActiveData;
};

/**
 * @}
 */
}//end namespace cx

#endif /* CXTRANSFERFUNCTION2DCOLORWIDGET_H_ */
