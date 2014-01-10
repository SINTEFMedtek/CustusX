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
#ifndef CXVIEWFOLLOWER_H
#define CXVIEWFOLLOWER_H

#include <QObject>
#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"
#include "sscBoundingBox3D.h"

namespace cx
{

/**
 * Ensure the tool is inside a given viewport, by moving the global center.
 *
 * \ingroup cx
 * \date 2014-01-14
 * \author christiana
 */
class ViewFollower : public QObject
{
	Q_OBJECT
public:
	void setSliceProxy(SliceProxyPtr sliceProxy);
	void setView(DoubleBoundingBox3D bb_s);

private slots:
	void ensureCenterWithinView();
private:
	Vector3D findCenterShift_s();
	DoubleBoundingBox3D findStaticBox();
	Vector3D findShiftFromBoxToTool_s(DoubleBoundingBox3D BB_s, Vector3D pt_s);
	void applyShiftToCenter(Vector3D shift_s);
	Vector3D findVirtualTooltip_s();

	SliceProxyPtr mSliceProxy;
	DoubleBoundingBox3D mBB_s;
private:
};


} // namespace cx

#endif // CXVIEWFOLLOWER_H
