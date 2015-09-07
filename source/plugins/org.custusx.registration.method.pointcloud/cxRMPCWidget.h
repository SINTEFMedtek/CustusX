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
#ifndef CXRMPCWIDGET_H
#define CXRMPCWIDGET_H

#include <QPushButton>
#include <QDomElement>
#include "cxRegistrationBaseWidget.h"
#include "cxForwardDeclarations.h"
#include "cxXmlOptionItem.h"


namespace cx
{
class WidgetObscuredListener;
class RecordTrackingWidget;
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
 * \brief Register a point cloud to a surface
 *
 * \date 2015-09-06
 * \author Christian Askeland
 */
class RMPCWidget: public RegistrationBaseWidget
{
	Q_OBJECT

public:
	RMPCWidget(RegServices services, QWidget *parent);
	virtual ~RMPCWidget()
	{
	}
	virtual QString defaultWhatsThis() const;
private slots:
	void registerSlot();
//	void acquisitionStarted();
//	void acquisitionStopped();
//	void obscuredSlot(bool obscured);

//	void acquisitionCancelled();
//	void recordedSessionsChanged();
private:
	RegServices mServices;
	QVBoxLayout* mVerticalLayout;
	QLabel* mLabel;
	XmlOptionFile mOptions;
	MeshPtr mMesh;

	QVBoxLayout* createVBoxInGroupBox(QVBoxLayout* parent, QString header);

//	RecordSessionWidgetPtr mRecordSessionWidget;
	StringPropertySelectMeshPtr mSurfaceSelector;
//	StringPropertyPtr mSessionSelector;
	QPushButton* mRegisterButton;
//	ToolPtr mTool;
//	StringPropertySelectToolPtr mToolSelector;
	RecordTrackingWidget* mRecordTrackingWidget;

//	boost::shared_ptr<WidgetObscuredListener> mObscuredListener;

//	ToolRep3DPtr getToolRepIn3DView(ToolPtr tool);

//	void initSessionSelector();
//	QStringList getSessionList();
//	void initializeTrackingService();
};

} //namespace cx

#endif // CXRMPCWIDGET_H
