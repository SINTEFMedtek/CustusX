/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CX2DSLIDER_H
#define CX2DSLIDER_H

#include "org_custusx_core_view_Export.h"

#include "ctkDoubleSlider.h"
#include "cxActiveImageProxy.h"
#include "cxTransform3D.h"

namespace cx
{
class org_custusx_core_view_EXPORT Slider2D : public QObject
{
	Q_OBJECT
public:
	Slider2D(VisServicesPtr services, SliceProxyPtr sliceProxy, ctkDoubleSlider *slider);

protected slots:
	virtual void imageChanged();
	virtual void updateSliderPosition();
	virtual void sliderChanged(double sliderValue);

protected:
	virtual ImagePtr getImage();
	virtual double getOutOfPlaneVoxels();
	virtual int getDimension();
	virtual PLANE_TYPE getPlaneType();
	virtual Vector3D get_tool_d();
	virtual bool correctSliderValue(double &sliderValue);
	virtual void updateSliderDiffIfOutOfRange(double &sliderValueDiff);
	virtual void shiftPosOutOfPlane(Vector3D delta_d_voxels);
	virtual Transform3D get_sMr();

	VisServicesPtr mServices;
	double mLastSliderValue;
	ctkDoubleSlider *mSlider = nullptr;
	ActiveImageProxyPtr mActiveImageProxy;
	ToolPtr mTool;
	SliceProxyPtr mSliceProxy;

};
typedef boost::shared_ptr<Slider2D> Slider2DPtr;
}//cx
#endif // CX2DSLIDER_H
