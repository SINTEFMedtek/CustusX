/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTESTTESTTRANSFERFUNCTIONCOLORWIDGET_H_
#define CXTESTTESTTRANSFERFUNCTIONCOLORWIDGET_H_

#include "cxtestgui_export.h"

#include "cxTransferFunctionColorWidget.h"

namespace cxtest {
/*
 * TestTransferfunctionColorWidget
 *
 *  \date Jul 29, 2013
 *  \author Ole Vegard Solberg, SINTEF
 */
class CXTESTGUI_EXPORT TestTransferFunctionColorWidget : public cx::TransferFunctionColorWidget
{
	Q_OBJECT
public:
	TestTransferFunctionColorWidget(cx::ActiveDataPtr activeData);

	void initWithTestData();
	void setPlotArea(QRect plotArea);
	int getLeftAreaBoundary();
	int getRigthAreaBoundary();
};

} /* namespace cxtest */
#endif /* CXTESTTESTTRANSFERFUNCTIONCOLORWIDGET_H_ */
