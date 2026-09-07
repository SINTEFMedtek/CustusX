/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXLIVERWIDGET_H_
#define CXLIVERWIDGET_H_

#include "org_custusx_liver_Export.h"
#include "cxBaseWidget.h"
#include "cxForwardDeclarations.h"

class QTabWidget;

namespace cx
{

class org_custusx_liver_EXPORT LiverWidget : public BaseWidget
{
	Q_OBJECT
public:
	explicit LiverWidget(VisServicesPtr services, QWidget* parent = 0);
	virtual ~LiverWidget();

	static QString getWidgetName();

private:
	QTabWidget* mTabWidget;
};

} /* namespace cx */

#endif /* CXLIVERWIDGET_H_ */
