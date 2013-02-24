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

/*
 * cxInteractiveCropper.h
 *
 *  \date Aug 24, 2010
 *      \author christiana
 */

#ifndef CXINTERACTIVECROPPER_H_
#define CXINTERACTIVECROPPER_H_

#include <QObject>
#include <QPointer>
#include <vtkSmartPointer.h>
#include "sscBoundingBox3D.h"
#include "sscForwardDeclarations.h"
#include "vtkForwardDeclarations.h"


namespace cx
{
typedef vtkSmartPointer<class CropBoxCallback> CropBoxCallbackPtr;
typedef vtkSmartPointer<class CropBoxEnableCallback> CropBoxEnableCallbackPtr;
typedef boost::shared_ptr<class ActiveImageProxy> ActiveImageProxyPtr;

/**
* \file
* \addtogroup cxServiceVisualization
* @{
*/

/**Helper class for cropping the active volume using a
 * bounding box.
 * The bounding box can be set either explicitly or using
 * a vtkBoxWidget.
 */
class InteractiveCropper: public QObject
{
Q_OBJECT
public:
	InteractiveCropper();
	void setView(ssc::ViewWidget* view); ///< adds an interactive box widget to the view. Press 'I' to show
	ssc::DoubleBoundingBox3D getBoundingBox(); ///< get BB in data space
	void setBoundingBox(const ssc::DoubleBoundingBox3D& bb_d); ///< set BB in reference space
	void resetBoundingBox(); ///< set bounding box back to initial size (entire volume)
	ssc::DoubleBoundingBox3D getMaxBoundingBox();
	bool getUseCropping();
	bool getShowBoxWidget() const;
signals:
	void changed();
public slots:
	void showBoxWidget(bool on);
	void useCropping(bool on);
private slots:
	void imageCropChangedSlot();
	void imageChangedSlot();

private:
	void boxWasShown(bool val);
	friend class CropBoxCallback;
	friend class CropBoxEnableCallback;
	void setBoxWidgetSize(const ssc::DoubleBoundingBox3D& bb_d);
	ssc::DoubleBoundingBox3D getBoxWidgetSize();
	void setCroppingRegion(ssc::DoubleBoundingBox3D bb_d);
	void updateBoxWidgetInteractor();
	void initialize();

	vtkVolumeMapperPtr getMapper();
	ssc::ImagePtr mImage;
	QPointer<ssc::ViewWidget> mView;
	//vtkBoxRepresentationPtr mBoxRep;
	//vtkBoxWidget2Ptr mBoxWidget2;
	vtkBoxWidgetPtr mBoxWidget;
	CropBoxCallbackPtr mCropBoxCallback;
	CropBoxEnableCallbackPtr mCropBoxEnableCallback;
	CropBoxEnableCallbackPtr mCropBoxDisableCallback;
	bool mShowBoxWidget;
	ActiveImageProxyPtr mActiveImageProxy;
};

typedef boost::shared_ptr<InteractiveCropper> InteractiveCropperPtr;

/**
* @}
*/
} // namespace cx

#endif /* CXINTERACTIVECROPPER_H_ */
