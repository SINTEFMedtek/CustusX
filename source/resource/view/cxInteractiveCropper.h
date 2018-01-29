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
