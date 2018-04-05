/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestViewServiceMockWithRenderWindowFactory.h"
#include "cxRenderWindowFactory.h"

namespace cxtest
{


ViewServiceMockWithRenderWindowFactory::ViewServiceMockWithRenderWindowFactory()
{
	mRenderWindowFactory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());
}

vtkRenderWindowPtr ViewServiceMockWithRenderWindowFactory::getRenderWindow(QString uid, bool offScreenRendering)
{
	return mRenderWindowFactory->getRenderWindow(uid, offScreenRendering);
}

vtkRenderWindowPtr ViewServiceMockWithRenderWindowFactory::getSharedRenderWindow() const
{
	return mRenderWindowFactory->getSharedRenderWindow();
}

cx::RenderWindowFactoryPtr ViewServiceMockWithRenderWindowFactory::getRenderWindowFactory() const
{
	return mRenderWindowFactory;
}

}//cxtest

