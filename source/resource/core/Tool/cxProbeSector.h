/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXPROBESECTOR_H_
#define CXPROBESECTOR_H_

#include "cxResourceExport.h"

#include <boost/shared_ptr.hpp>
#include <QSize>
#include "vtkSmartPointer.h"
#include "vtkForwardDeclarations.h"
#include "cxProbeDefinition.h"
#include "cxTransform3D.h"

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkPolyData> vtkPolyDataPtr;
typedef vtkSmartPointer<class vtkPolyLine> vtkPolyLinePtr;

namespace cx
{

typedef boost::shared_ptr<class ProbeSector> ProbeSectorPtr;

/** \brief Utility functions for drawing an US Probe sector
 *
 * \ingroup cx_resource_core_tool
 */
class cxResource_EXPORT ProbeSector
{
public:
	ProbeSector();
	void setData(ProbeDefinition data);

	vtkImageDataPtr getMask();
	vtkPolyDataPtr getSector(); ///< get a polydata representation of the us sector
	vtkPolyDataPtr getSectorLinesOnly(); ///< get a polydata representation of the us sector
	vtkPolyDataPtr getSectorSectorOnlyLinesOnly(); ///< get a polydata representation of the us sector
	vtkPolyDataPtr getClipRectLinesOnly(); ///< get a polydata representation of the us clip rect
	vtkPolyDataPtr getOriginPolyData(); ///< get a polydata representation of the origin
	Transform3D get_tMu() const; ///< get transform from image space u to probe tool space t.
	Transform3D get_uMv() const; ///< get transform from inverted image space v (origin in ul corner) to image space u.
	void updateSector();

	ProbeDefinition mData;

	void test();

private:
	vtkPolyDataPtr getClipRectPolyData(); ///< generate a polydata containing only a polygon representing the sector cliprect.
	bool clipRectIntersectsSector() const;

	bool isInside(Vector3D p_u);
	vtkPolyDataPtr mPolyData; ///< polydata representation of the probe, in space u
};

} // namespace cx

#endif /*SSCPROBEDECTOR_H_*/
