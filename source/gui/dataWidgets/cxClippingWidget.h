/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXCLIPPINGWIDGET_H_
#define CXCLIPPINGWIDGET_H_

#include "cxGuiExport.h"

#include "cxBaseWidget.h"
#include "cxForwardDeclarations.h"
class QCheckBox;

namespace cx
{
typedef boost::shared_ptr<class StringPropertySelectData> StringPropertySelectDataPtr;

/**
 * \file
 * \addtogroup cx_gui
 * @{
 */


/*
 * \class ClippingWidget
 *
 * \date Aug 25, 2010
 * \author Christian Askeland, SINTEF
 */

class cxGui_EXPORT ClippingWidget: public BaseWidget
{
Q_OBJECT

public:
	ClippingWidget(VisServicesPtr services, QWidget* parent);

private:
	InteractiveClipperPtr mInteractiveClipper;

	QCheckBox* mUseClipperCheckBox;
	QCheckBox* mInvertPlaneCheckBox;
	StringPropertyBasePtr mPlaneAdapter;
	StringPropertySelectDataPtr mDataAdapter;
	VisServicesPtr mServices;
private slots:
	void setupUI();
	void clipperChangedSlot();
	void clearButtonClickedSlot();
	void saveButtonClickedSlot();
	void imageChangedSlot();
};

/**
 * @}
 */
}//namespace cx

#endif /* CXCLIPPINGWIDGET_H_ */
