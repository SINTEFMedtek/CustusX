/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTESTVIEWSERVICEMOCKWITHRENDERWINDOWFACTORY_H
#define CXTESTVIEWSERVICEMOCKWITHRENDERWINDOWFACTORY_H


#include "cxtestresourcevisualization_export.h"
#include "cxViewServiceNull.h"

namespace cxtest
{
typedef boost::shared_ptr<class ViewServiceMockWithRenderWindowFactory> ViewServiceMocWithRenderWindowFactoryPtr;

class CXTESTRESOURCEVISUALIZATION_EXPORT ViewServiceMockWithRenderWindowFactory : public cx::ViewServiceNull
{
public:
	ViewServiceMockWithRenderWindowFactory();
	virtual vtkRenderWindowPtr getRenderWindow(QString uid, bool offScreenRendering = false);
	virtual vtkRenderWindowPtr getSharedRenderWindow() const;
	cx::RenderWindowFactoryPtr getRenderWindowFactory() const;
private:
	cx::RenderWindowFactoryPtr mRenderWindowFactory;
};
}//cxtest

#endif // CXTESTVIEWSERVICEMOCKWITHRENDERWINDOWFACTORY_H
