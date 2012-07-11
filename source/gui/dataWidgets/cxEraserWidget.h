// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXERASERWIDGET_H_
#define CXERASERWIDGET_H_

#include "cxBaseWidget.h"

#include "sscVector3D.h"
#include "vtkForwardDeclarations.h"
#include "sscDoubleDataAdapterXml.h"
typedef vtkSmartPointer<class vtkSphereWidget> vtkSphereWidgetPtr;

namespace cx
{

/**
 * \file
 * \addtogroup cxGUI
 * @{
 */



/**\brief Widget for erasing parts of images/meshes.
 *
 *
 * \date Mar 30, 2012
 * \author Christian Askeland, SINTEF
 */
class EraserWidget: public BaseWidget
{
Q_OBJECT

public:
	EraserWidget(QWidget* parent);
	virtual QString defaultWhatsThis() const;

	virtual ~EraserWidget();
private:
	QCheckBox* mContinousEraseCheckBox;
	vtkSphereSourcePtr mSphere;
	QCheckBox* mShowEraserCheckBox;
	ssc::DoubleDataAdapterXmlPtr mSphereSizeAdapter;
	QAction* mDuplicateAction;
	QAction* mSaveAction;
	QAction* mRemoveAction;
	QWidget* mSphereSize;

	void enableButtons();
	template <class TYPE>
	void eraseVolume(TYPE* volumePointer, TYPE replaceVal);

	QTimer* mContinousEraseTimer;

	ssc::Vector3D mPreviousCenter;
	double mPreviousRadius;

private slots:
	void toggleShowEraser(bool on);
	void toggleContinous(bool on);
	void saveSlot();
	void removeSlot();
	void continousRemoveSlot();
	void duplicateSlot();
	void sphereSizeChangedSlot();
};

}

#endif /* CXERASERWIDGET_H_ */
