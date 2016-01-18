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

#ifndef CXBRONCHOSCOPYREGISTRATIONWIDGET_H
#define CXBRONCHOSCOPYREGISTRATIONWIDGET_H

#include <QPushButton>
#include <QDomElement>
#include "cxRegistrationBaseWidget.h"
#include "cxForwardDeclarations.h"
#include "cxXmlOptionItem.h"


namespace cx
{
class RecordTrackingWidget;
class WidgetObscuredListener;
typedef boost::shared_ptr<class Acquisition> AcquisitionPtr;
typedef boost::shared_ptr<class StringPropertySelectMesh> StringPropertySelectMeshPtr;
typedef boost::shared_ptr<class ToolRep3D> ToolRep3DPtr;
typedef boost::shared_ptr<class RecordSessionWidget> RecordSessionWidgetPtr;
typedef boost::shared_ptr<class AcquisitionData> AcquisitionDataPtr;
typedef boost::shared_ptr<class BronchoscopyRegistration> BronchoscopyRegistrationPtr;
typedef std::map<QString, ToolPtr> ToolMap;
typedef boost::shared_ptr<class StringPropertySelectTool> StringPropertySelectToolPtr;

/**
 * BronchoscopyRegistrationWidget
 *
 * \brief Register tracked bronchostopy tool path to lung centerline data (from CT)
 *
 * \date Oct 10, 2013
 * \author Ole Vegard Solberg
 * \author Erlend Hofstad
 */
class BronchoscopyRegistrationWidget: public RegistrationBaseWidget
{
	Q_OBJECT

	BronchoscopyRegistrationPtr mBronchoscopyRegistration;

public:
	BronchoscopyRegistrationWidget(RegServicesPtr services, QWidget *parent);
	virtual ~BronchoscopyRegistrationWidget()
	{
	}
	virtual QString defaultWhatsThis() const;

protected:
	virtual void prePaintEvent();
private slots:
	void processCenterlineSlot();
	void registerSlot();
	void clearDataOnNewPatient();
private:
	void setup();

	RegServicesPtr mServices;
	QVBoxLayout* mVerticalLayout;
	BoolPropertyPtr mUseLocalRegistration;
	BoolPropertyPtr mUseSubsetOfGenerations;
	DoublePropertyPtr mMaxNumberOfGenerations;
	DoublePropertyPtr mMaxLocalRegistrationDistance;
	XmlOptionFile mOptions;
	MeshPtr mMesh;

	StringPropertySelectMeshPtr mSelectMeshWidget;
	QPushButton* mProcessCenterlineButton;
	QPushButton* mRegisterButton;
    ToolPtr mTool;

	RecordTrackingWidget* mRecordTrackingWidget;

	void initializeTrackingService();

	void createMaxNumberOfGenerations(QDomElement root);
	void selectSubsetOfBranches(QDomElement root);
	void useLocalRegistration(QDomElement root);
	void createMaxLocalRegistrationDistance(QDomElement root);
};

} //namespace cx

#endif // CXBRONCHOSCOPYREGISTRATIONWIDGET_H
