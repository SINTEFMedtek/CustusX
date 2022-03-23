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

#ifndef CXBRONCHOSCOPYNAVIGATIONWIDGET_H_
#define CXBRONCHOSCOPYNAVIGATIONWIDGET_H_

#include <QtWidgets>
#include <QWidget>
#include <QPushButton>
#include "qdom.h"
#include "cxRegistrationBaseWidget.h"
#include "cxResourceWidgetsExport.h"
#include "cxBaseWidget.h"
#include "cxSelectDataStringProperty.h"
#include "cxMesh.h"
#include "cxXmlOptionItem.h"
#include "cxCheckBoxWidget.h"

class QVBoxLayout;


namespace cx
{

typedef boost::shared_ptr<class TrackingSystemBronchoscopyService> TrackingSystemBronchoscopyServicePtr;
typedef boost::shared_ptr<class BronchoscopePositionProjection> BronchoscopePositionProjectionPtr;
typedef boost::shared_ptr<class StringPropertySelectTool> StringPropertySelectToolPtr;

/**
 * Widget for use in the BronchoscopyNavigation
 *
 * \ingroup org_custusx_bronchoscopynavigation
 *
 * \date 2014-10-30
 * \author Erlend Hofstad
 */
class BronchoscopyNavigationWidget : public BaseWidget
{
	Q_OBJECT
public:
	BronchoscopyNavigationWidget(VisServicesPtr services, QWidget* parent = 0);
	virtual ~BronchoscopyNavigationWidget();

private slots:
	void processCenterlineSlot();
	void enableSlot();
	void disableSlot();
	void showAdvancedOptionsSlot();

private:
	QString defaultWhatsThis() const;
	QVBoxLayout*  mVerticalLayout;

BoolPropertyPtr mUseAdvancedCenterlineProjection;

	StringPropertySelectToolPtr mToolSelector;
	StringPropertySelectMeshPtr mSelectMeshWidget;
	QPushButton* mProcessCenterlineButton;
	QWidget* mMaxSearchDistanceWidget;
	QWidget* mAlphaWidget;
	QPushButton* mEnableButton;
	QPushButton* mDisableButton;
	QCheckBox* mAdvancedOption;
	ToolPtr mTool;
	TrackingSystemBronchoscopyServicePtr mTrackingSystem;
	BronchoscopePositionProjectionPtr mProjectionCenterlinePtr;

	bool mIsCenerlineProcessed;

	PatientModelServicePtr mPatientModelService;
	ViewServicePtr mViewService;
	TrackingServicePtr mTrackingService;

	XmlOptionFile mOptions;
};
} /* namespace cx */

#endif /* CXBRONCHOSCOPYNAVIGATIONWIDGET_H_ */
