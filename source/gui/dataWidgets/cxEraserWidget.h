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

#ifndef CXERASERWIDGET_H_
#define CXERASERWIDGET_H_

#include "cxGuiExport.h"

#include "cxBaseWidget.h"

#include "cxVector3D.h"
#include "vtkForwardDeclarations.h"
#include "cxDoubleProperty.h"
#include "cxActiveImageProxy.h"

typedef vtkSmartPointer<class vtkSphereWidget> vtkSphereWidgetPtr;

namespace cx
{

/**
 * \file
 * \addtogroup cx_gui
 * @{
 */



/**\brief Widget for erasing parts of images/meshes.
 *
 *
 * \date Mar 30, 2012
 * \author Christian Askeland, SINTEF
 */
class cxGui_EXPORT EraserWidget: public BaseWidget
{
Q_OBJECT

public:
	EraserWidget(PatientModelServicePtr patientModelService, ViewServicePtr viewService, QWidget* parent);

	virtual ~EraserWidget();
private:
	QCheckBox* mContinousEraseCheckBox;
	vtkSphereSourcePtr mSphere;
	QCheckBox* mShowEraserCheckBox;
	DoublePropertyPtr mSphereSizeAdapter;
	DoublePropertyPtr mEraseValueAdapter;
	QAction* mDuplicateAction;
	QAction* mSaveAction;
	QAction* mRemoveAction;
	QWidget* mSphereSize;
	QWidget* mEraseValueWidget;

	void enableButtons();
	template <class TYPE>
	void eraseVolume(TYPE* volumePointer);

	QTimer* mContinousEraseTimer;

	Vector3D mPreviousCenter;
	double mPreviousRadius;

	ActiveImageProxyPtr mActiveImageProxy;
	PatientModelServicePtr mPatientModelService;
	ViewServicePtr mViewService;
	ActiveDataPtr mActiveData;

private slots:
	void toggleShowEraser(bool on);
	void toggleContinous(bool on);
	void saveSlot();
	void removeSlot();
	void continousRemoveSlot();
	void duplicateSlot();
	void sphereSizeChangedSlot();
	void activeImageChangedSlot();
};

}

#endif /* CXERASERWIDGET_H_ */
