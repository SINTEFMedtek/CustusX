/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSAMPLERWIDGET_H
#define CXSAMPLERWIDGET_H

#include "cxGuiExport.h"

#include "cxBaseWidget.h"

#include "cxSpaceProperty.h"
#include "cxVector3DProperty.h"
#include "cxActiveToolProxy.h"
#include "cxPointMetric.h"

namespace cx
{

/** 
 * Utility for sampling the current tool point.
 * Displays the current tool tip position in a selected coordinate system.
 *
 * \ingroup cx_gui
 * \date feb 1, 2013, 2013
 * \author christiana
 */
class cxGui_EXPORT SamplerWidget : public BaseWidget
{
	Q_OBJECT
public:
	SamplerWidget(TrackingServicePtr trackingService, PatientModelServicePtr patientModelService, SpaceProviderPtr spaceProvider, QWidget* parent);
	virtual ~SamplerWidget();

private slots:
	void spacesChangedSlot();
	void toggleAdvancedSlot();
protected:
	virtual void prePaintEvent();
private:
	void showAdvanced();

	QHBoxLayout* mLayout;
	SpacePropertyPtr mSpaceSelector;
	SpaceListenerPtr mListener;
	ActiveToolProxyPtr mActiveTool;
	QLineEdit* mCoordLineEdit;
	QAction* mAdvancedAction;
	QHBoxLayout* mAdvancedLayout;
	QWidget* mAdvancedWidget;
	PatientModelServicePtr mPatientModelService;
	SpaceProviderPtr mSpaceProvider;
};

} // namespace cx

#endif // CXSAMPLERWIDGET_H
