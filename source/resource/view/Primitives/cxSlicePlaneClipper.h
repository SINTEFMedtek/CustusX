/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXSLICEPLANECLIPPER_H_
#define CXSLICEPLANECLIPPER_H_

#include "cxResourceVisualizationExport.h"

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
 * \ingroup cx_resource_view

 */
class cxResourceVisualization_EXPORT SlicePlaneClipper : public QObject
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
