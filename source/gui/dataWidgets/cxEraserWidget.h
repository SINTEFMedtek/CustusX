/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

protected slots:
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
