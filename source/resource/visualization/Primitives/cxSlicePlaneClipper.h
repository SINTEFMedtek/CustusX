// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef CXSLICEPLANECLIPPER_H_
#define CXSLICEPLANECLIPPER_H_

#include <set>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <vtkSmartPointer.h>
#include <QObject>
#include "cxForwardDeclarations.h"
#include "cxVector3D.h"
#include "vtkForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class SlicePlaneClipper> SlicePlaneClipperPtr;

/*
 * \date Aug 20, 2010
 * \author christiana
 */

/** \brief Clip several 3D volumes using a SliceProxy.
 *
 * Several 3D volumes, represented as VolumetricBaseRep, are decorated with
 * a clip plane defined by a SliceProxy.
 *
 * \ingroup cx_resource_visualization

 */
class SlicePlaneClipper : public QObject
{
	Q_OBJECT
public:
	typedef std::set<VolumetricBaseRepPtr> VolumesType;

	static SlicePlaneClipperPtr New();
	~SlicePlaneClipper();
	void setSlicer(SliceProxyPtr slicer);
	SliceProxyPtr getSlicer();
	void setInvertPlane(bool on);
	bool getInvertPlane() const;
	vtkPlanePtr getClipPlaneCopy();
	vtkPlanePtr getClipPlane();

signals:
	void slicePlaneChanged();
private slots:
	void changedSlot();
private:
	SlicePlaneClipper();
	void updateClipPlane();
	Vector3D getUnitNormal() const;
	SliceProxyPtr mSlicer;
	bool mInvertPlane;

	vtkPlanePtr mClipPlane;
};

} // namespace cx

#endif /* CXSLICEPLANECLIPPER_H_ */
