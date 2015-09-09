/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/
#ifndef CXRECORDTRACKINGWIDGET_H
#define CXRECORDTRACKINGWIDGET_H

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
typedef boost::shared_ptr<class RecordSessionWidget> RecordSessionWidgetPtr;
typedef boost::shared_ptr<class AcquisitionData> AcquisitionDataPtr;
//typedef boost::shared_ptr<class BronchoscopyRegistration> BronchoscopyRegistrationPtr;
typedef std::map<QString, ToolPtr> ToolMap;
typedef boost::shared_ptr<class StringPropertySelectTool> StringPropertySelectToolPtr;

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
	RecordTrackingWidget(XmlOptionFile options, AcquisitionServicePtr acquisitionService, VisServices services, QWidget *parent);
	virtual ~RecordTrackingWidget()	{}

	ToolPtr getSuitableRecordingTool();
	TimedTransformMap getRecordedTrackerData_prMt();
	void ShowLastRecordingInView();
	StringPropertyPtr getSessionSelector() { return mSessionSelector; }

private slots:
	void acquisitionStarted();
	void acquisitionStopped();
	void obscuredSlot(bool obscured);

	void acquisitionCancelled();
	void recordedSessionsChanged();
private:
	VisServices mServices;
	AcquisitionServicePtr mAcquisitionService;
	XmlOptionFile mOptions;

	RecordSessionWidgetPtr mRecordSessionWidget;
	StringPropertyPtr mSessionSelector;
	ToolPtr mRecordingTool;
	StringPropertySelectToolPtr mToolSelector;

	boost::shared_ptr<WidgetObscuredListener> mObscuredListener;

	void initSessionSelector();
	QStringList getSessionList();
//	void initializeTrackingService();
	ToolRep3DPtr getToolRepIn3DView();
};

} //namespace cx

#endif // CXRECORDTRACKINGWIDGET_H
