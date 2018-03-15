/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXIMAGEPROPERTIESWIDGET_H
#define CXIMAGEPROPERTIESWIDGET_H

#include "cxGuiExport.h"

#include <QtWidgets>

#include "cxBaseWidget.h"
#include "cxActiveImageProxy.h"
#include "cxTabbedWidget.h"

class QComboBox;

namespace cx
{

/**
 * \brief Widget for displaying and manipulating various Image properties.
 * \ingroup cx_gui
 *
 */
class cxGui_EXPORT ImagePropertiesWidget : public TabbedWidget
{
	Q_OBJECT
public:
	ImagePropertiesWidget(VisServicesPtr services, QWidget* parent);
	virtual ~ImagePropertiesWidget() {}
};

}//end namespace cx

#endif // CXIMAGEPROPERTIESWIDGET_H
