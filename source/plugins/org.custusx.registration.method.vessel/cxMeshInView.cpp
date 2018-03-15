/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxMeshInView.h"

#include "cxMesh.h"
#include "cxGeometricRep.h"
#include "cxViewService.h"
#include "cxView.h"
#include "cxLogger.h"

namespace cx
{

MeshInView::MeshInView(ViewServicePtr viewService) : mViewService(viewService)
{

}
MeshInView::~MeshInView()
{
	this->hide();
}

void MeshInView::show(vtkPolyDataPtr polyData)
{
	if (!mRep)
	{
		MeshPtr mesh(new Mesh("mesh", "mesh"));
		mesh->setColor(QColor("cornflowerblue"));

		mRep = GeometricRep::New();
		mRep->setMesh(mesh);

		ViewPtr view = mViewService->get3DView();
		if (view)
			view->addRep(mRep);
	}

	mRep->getMesh()->setVtkPolyData(polyData);
}

void MeshInView::hide()
{
	ViewPtr view = mViewService->get3DView();
	if (view)
		view->removeRep(mRep);
	mRep.reset();
}

} // namespace cx
