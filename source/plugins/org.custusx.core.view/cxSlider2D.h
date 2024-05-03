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
#include "cxActiveToolProxy.h"

namespace cx
{
class org_custusx_core_view_EXPORT Slider2D : public QObject
{
	Q_OBJECT
public:
	Slider2D(VisServicesPtr services, SliceProxyPtr sliceProxy, ctkDoubleSlider *slider);

protected slots:
	void imageChanged();
	void updateSliderPosition();
	void sliderChanged(double sliderValue);

protected:
	double getOutOfPlaneVoxels();
	int getDimension();
	Vector3D get_tool_d();
	bool correctSliderValue(double &sliderValue);
	void updateSliderDiffIfOutOfRange(double &sliderValueDiff);
	void shiftPosOutOfPlane(Vector3D delta_d_voxels);

	VisServicesPtr mServices;
	int mLastSliderValue;
	ctkDoubleSlider *mSlider = nullptr;
	ActiveImageProxyPtr mActiveImageProxy;
	ActiveToolProxyPtr mActiveTool;
	SliceProxyPtr mSliceProxy;

};
typedef boost::shared_ptr<Slider2D> Slider2DPtr;
}//cx
#endif // CX2DSLIDER_H
