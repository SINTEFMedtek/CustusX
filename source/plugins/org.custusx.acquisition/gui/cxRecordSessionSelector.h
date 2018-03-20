/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXRECORDSESSIONSELECTOR_H_
#define CXRECORDSESSIONSELECTOR_H_

#include <QPushButton>
#include <QDomElement>
#include "cxForwardDeclarations.h"
#include "cxXmlOptionItem.h"
#include "cxTool.h"
#include "cxVisServices.h"
#include "org_custusx_acquisition_Export.h"

namespace cx
{
class WidgetObscuredListener;
typedef boost::shared_ptr<class Acquisition> AcquisitionPtr;
typedef boost::shared_ptr<class StringPropertySelectMesh> StringPropertySelectMeshPtr;
typedef boost::shared_ptr<class ToolRep3D> ToolRep3DPtr;
typedef boost::shared_ptr<class RecordSession> RecordSessionPtr;
class RecordSessionWidget;
typedef boost::shared_ptr<class AcquisitionData> AcquisitionDataPtr;
//typedef boost::shared_ptr<class BronchoscopyRegistration> BronchoscopyRegistrationPtr;
typedef std::map<QString, ToolPtr> ToolMap;
typedef boost::shared_ptr<class StringPropertySelectTool> StringPropertySelectToolPtr;
typedef boost::shared_ptr<class SelectRecordSession> SelectRecordSessionPtr;

/**
 *
 * Wrap a SessionSelector, always show tracking data in 3D view for that session,
 * for a given tool.
 *
 * \date 2015-09-10
 * \author Christian Askeland
 */
class org_custusx_acquisition_EXPORT SelectRecordSession: public QObject
{
	Q_OBJECT

public:
	SelectRecordSession(XmlOptionFile options,
						 AcquisitionServicePtr acquisitionService,
						 VisServicesPtr services);
	virtual ~SelectRecordSession();

	void setTool(ToolPtr tool);
	ToolPtr getTool();
	void setVisible(bool on);
	TimedTransformMap getRecordedTrackerData_prMt();
	StringPropertyPtr getSessionSelector() { return mSessionSelector; }

private slots:

	void recordedSessionsChanged();
private:
	VisServicesPtr mServices;
	AcquisitionServicePtr mAcquisitionService;
	XmlOptionFile mOptions;
	ToolPtr mCurrentTracedTool;

	StringPropertyPtr mSessionSelector;
	ToolPtr mToolOverride;
	bool mVisible;

	void initSessionSelector();
	ToolRep3DPtr getToolRepIn3DView(ToolPtr tool);
	void showSelectedRecordingInView();
	void clearTracer();
	RecordSessionPtr getSession();
	void warnIfNoTrackingDataInSession();
	ToolPtr findToolContainingMostDataForSession(std::map<QString,ToolPtr> tools, RecordSessionPtr session);
	void updateHelpText();
};

} //namespace cx


#endif // CXRECORDSESSIONSELECTOR_H_
