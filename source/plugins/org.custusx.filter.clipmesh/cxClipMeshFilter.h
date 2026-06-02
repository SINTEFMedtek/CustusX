/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXCLIPMESHFILTER_H
#define CXCLIPMESHFILTER_H

#include "org_custusx_filter_clipmesh_Export.h"

#include "cxFilterImpl.h"
#include "cxDoubleProperty.h"
#include "vtkForwardDeclarations.h"

namespace cx
{

/** Filter that clips a mesh to the non-zero region of a volume.
 *
 * Each mesh vertex is probed against the volume. Vertices (and the triangles
 * that connect them) where the volume value is above a user-specified threshold
 * are kept; the rest are removed.  The result is stored as a new mesh.
 */
class org_custusx_filter_clipmesh_EXPORT ClipMeshFilter : public FilterImpl
{
	Q_OBJECT
	Q_INTERFACES(cx::Filter)

public:
	ClipMeshFilter(VisServicesPtr services);

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;

	virtual bool execute();
	virtual bool postProcess();

	DoublePropertyPtr getThresholdOption(QDomElement root);

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

private:
	vtkPolyDataPtr mClippedPolyData;
};

} // namespace cx

#endif // CXCLIPMESHFILTER_H
