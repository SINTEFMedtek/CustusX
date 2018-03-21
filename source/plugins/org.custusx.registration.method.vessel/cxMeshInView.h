/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXMESHINVIEW_H
#define CXMESHINVIEW_H

#include "cxForwardDeclarations.h"
#include "org_custusx_registration_method_vessel_Export.h"

namespace cx
{

typedef boost::shared_ptr<class MeshInView> MeshInViewPtr;

/**
 * Display a mesh or polydata in the main 3D view.
 */
class org_custusx_registration_method_vessel_EXPORT MeshInView
{
public:
	MeshInView(ViewServicePtr viewService);
	~MeshInView();
	void show(vtkPolyDataPtr polyData);
	void hide();

private:
	GeometricRepPtr mRep;
	ViewServicePtr mViewService;
};

} // namespace cx


#endif // CXMESHINVIEW_H
