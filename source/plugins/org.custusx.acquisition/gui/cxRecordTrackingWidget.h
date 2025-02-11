/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXRECORDTRACKINGWIDGET_H
#define CXRECORDTRACKINGWIDGET_H

#include <QPushButton>
#include <QDomElement>
#include <QDateTime>
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
class RecordSessionWidget;
typedef boost::shared_ptr<class AcquisitionData> AcquisitionDataPtr;
//typedef boost::shared_ptr<class BronchoscopyRegistration> BronchoscopyRegistrationPtr;
typedef std::map<QString, ToolPtr> ToolMap;
typedef boost::shared_ptr<class SelectRecordSession> SelectRecordSessionPtr;

#define ADJUST_TRACKING_DATA_OFFSET 10 //seconds

/**
 *
 * Record tracking data.
 *
 * \date 2015-09-06
 * \author Christian Askeland
 */
class org_custusx_acquisition_EXPORT RecordTrackingWidget: public QWidget
{
	Q_OBJECT

public:
	RecordTrackingWidget(XmlOptionFile options,
						 AcquisitionServicePtr acquisitionService,
						 VisServicesPtr services,
						 QString category,
						 QWidget *parent,
						 bool useAdjustTracingData = false);
	virtual ~RecordTrackingWidget()	{}

	ToolPtr getSuitableRecordingTool();
	void useBaseToolIfAvailable(bool useBaseTool);
	TimedTransformMap getRecordedTrackerData_prMt();
	TimedTransformMap getRecordedTrackerData_prMs();
	StringPropertyPtr getSessionSelector();
	SelectRecordSessionPtr getSelectRecordSession() { return mSelectRecordSession; }
	void displayToolSelector(bool on);
	void hideMergeWithExistingSession();
	void drawAcquisitionIn3D(bool draw);

signals:
	void acquisitionCompleted(); ///< aquisition complete, and widget internal state is updated accordingly

private slots:
	void acquisitionStarted();
	void acquisitionStopped();
	void acquisitionCancelled();
	void onRecordSessionChanged();
	void onAdjustTrackingDataTimeIntervaChanged();
	void obscuredSlot(bool obscured);

private:
	VisServicesPtr mServices;
	AcquisitionServicePtr mAcquisitionService;
	XmlOptionFile mOptions;

	RecordSessionWidget* mRecordSessionWidget;
	QWidget* mToolSelectorWidget;
	QWidget* mMergeWithExistingSessionWidget;
	ToolPtr mRecordingTool;
	SelectRecordSessionPtr mSelectRecordSession;
	StringPropertySelectToolPtr mToolSelector;
	BoolPropertyPtr mMergeWithExistingSession;
	bool mDrawAqquisitionIn3D;
	bool mUseAdjustTrackingData;
	DoublePairPropertyPtr mAdjustTrackingDataTimeInterval;
	std::pair<QDateTime, QDateTime> mStartStopTimeBeforeAdjustment;

	boost::shared_ptr<WidgetObscuredListener> mObscuredListener;

	ToolRep3DPtr getToolRepIn3DView();
	void onToolChanged();
	void onMergeChanged();
	void createAdjustTrackingDataTimeInterval(QDomElement root);

};

} //namespace cx

#endif // CXRECORDTRACKINGWIDGET_H
