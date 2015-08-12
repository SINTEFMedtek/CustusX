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
