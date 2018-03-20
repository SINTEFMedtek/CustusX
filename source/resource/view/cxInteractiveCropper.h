/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

/*
 * cxInteractiveCropper.h
 *
 *  \date Aug 24, 2010
 *      \author christiana
 */

#ifndef CXINTERACTIVECROPPER_H_
#define CXINTERACTIVECROPPER_H_

#include "cxResourceVisualizationExport.h"

#include <QObject>
#include <QPointer>
#include <vtkSmartPointer.h>
#include "cxBoundingBox3D.h"
#include "cxForwardDeclarations.h"
#include "vtkForwardDeclarations.h"


namespace cx
{
typedef vtkSmartPointer<class CropBoxCallback> CropBoxCallbackPtr;
typedef vtkSmartPointer<class CropBoxEnableCallback> CropBoxEnableCallbackPtr;
typedef boost::shared_ptr<class ActiveImageProxy> ActiveImageProxyPtr;

/**
* \file
* \addtogroup cx_resource_view
* @{
*/

/**Helper class for cropping the active volume using a
 * bounding box.
 * The bounding box can be set either explicitly or using
 * a vtkBoxWidget.
 */
class cxResourceVisualization_EXPORT InteractiveCropper: public QObject
{
Q_OBJECT
public:
	explicit InteractiveCropper(ActiveDataPtr activeData);
	void setView(ViewPtr view); ///< adds an interactive box widget to the view. Press 'I' to show
	DoubleBoundingBox3D getBoundingBox(); ///< get BB in data space
	void setBoundingBox(const DoubleBoundingBox3D& bb_d); ///< set BB in reference space
	void resetBoundingBox(); ///< set bounding box back to initial size (entire volume)
	DoubleBoundingBox3D getMaxBoundingBox();
	bool getUseCropping();
	bool getShowBoxWidget() const;
	std::vector<int>  getDimensions();
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
	void setBoxWidgetSize(const DoubleBoundingBox3D& bb_d);
	DoubleBoundingBox3D getBoxWidgetSize();
	void setCroppingRegion(DoubleBoundingBox3D bb_d);
	void updateBoxWidgetInteractor();
	void initialize();

	ImagePtr mImage;
	ViewPtr mView;
	vtkBoxWidgetPtr mBoxWidget;
	CropBoxCallbackPtr mCropBoxCallback;
	CropBoxEnableCallbackPtr mCropBoxEnableCallback;
	CropBoxEnableCallbackPtr mCropBoxDisableCallback;
	bool mShowBoxWidget;
	ActiveImageProxyPtr mActiveImageProxy;
	ActiveDataPtr mActiveData;
};

/**
* @}
*/
} // namespace cx

#endif /* CXINTERACTIVECROPPER_H_ */
