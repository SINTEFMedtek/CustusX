/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRACKINGTESTWIDGET_H_
#define CXTRACKINGTESTWIDGET_H_

#include "cxGuiExport.h"

#include "cxTabbedWidget.h"
#include "cxForwardDeclarations.h"

namespace cx
{

/** \brief Tools for testing tracking: the tracking state, recording positions, and comparing tracking systems.
 *
 * \ingroup cx_gui
 */
class cxGui_EXPORT TrackingTestWidget : public TabbedWidget
{
	Q_OBJECT
public:
	TrackingTestWidget(TrackingServicePtr trackingService, VideoServicePtr videoService, QWidget* parent = NULL);
	virtual ~TrackingTestWidget();
};

} // namespace cx

#endif /* CXTRACKINGTESTWIDGET_H_ */
