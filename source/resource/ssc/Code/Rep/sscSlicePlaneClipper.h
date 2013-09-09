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

#ifndef SSCSLICEPLANECLIPPER_H_
#define SSCSLICEPLANECLIPPER_H_

#include <set>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <vtkSmartPointer.h>
#include <QObject>
#include "sscForwardDeclarations.h"
#include "sscVector3D.h"
#include "vtkForwardDeclarations.h"

namespace ssc
{
typedef boost::shared_ptr<class SlicePlaneClipper> SlicePlaneClipperPtr;

/*
 * \date Aug 20, 2010
 * \author christiana
 */

/**\brief Clip several 3D volumes using a SliceProxy.
 *
 * Several 3D volumes, represented as VolumetricBaseRep, are decorated with
 * a clip plane defined by a SliceProxy.
 *
 * \ingroup sscRep
 */
class SlicePlaneClipper : public QObject
{
	Q_OBJECT
public:
	typedef std::set<ssc::VolumetricBaseRepPtr> VolumesType;

	static SlicePlaneClipperPtr New();
	~SlicePlaneClipper();
	void setSlicer(ssc::SliceProxyPtr slicer);
	ssc::SliceProxyPtr getSlicer();
	void setInvertPlane(bool on);
	bool getInvertPlane() const;
	vtkPlanePtr getClipPlaneCopy();
	vtkPlanePtr getClipPlane();

	private slots:
	void changedSlot();
private:
	SlicePlaneClipper();
	void updateClipPlane();
	ssc::Vector3D getUnitNormal() const;
	ssc::SliceProxyPtr mSlicer;
	bool mInvertPlane;

	vtkPlanePtr mClipPlane;
};

} // namespace ssc

#endif /* SSCSLICEPLANECLIPPER_H_ */
