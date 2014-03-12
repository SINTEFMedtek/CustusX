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

#ifndef SSCPROBESECTOR_H_
#define SSCPROBESECTOR_H_

#include <boost/shared_ptr.hpp>
#include <QSize>
#include "vtkSmartPointer.h"
#include "vtkForwardDeclarations.h"
#include "sscProbeData.h"
#include "sscTransform3D.h"

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkPolyData> vtkPolyDataPtr;
typedef vtkSmartPointer<class vtkPolyLine> vtkPolyLinePtr;

namespace cx
{

typedef boost::shared_ptr<class ProbeSector> ProbeSectorPtr;

/**\brief Utility functions for drawing an US Probe sector
 *
 * \ingroup cx_resource_core_tool
 */
class ProbeSector
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
