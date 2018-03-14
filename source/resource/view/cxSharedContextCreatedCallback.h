/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSHAREDCONTEXTCREATEDCALLBACK_H
#define CXSHAREDCONTEXTCREATEDCALLBACK_H

#include "cxResourceVisualizationExport.h"

#include <vtkSmartPointer.h>
#include <vtkCommand.h>
#include "cxViewService.h"
#include "cxForwardDeclarations.h"

namespace cx
{
typedef vtkSmartPointer<class SharedContextCreatedCallback> SharedContextCreatedCallbackPtr;

class cxResourceVisualization_EXPORT SharedContextCreatedCallback : public vtkCommand
{

public:
	static SharedContextCreatedCallback *New();
	SharedContextCreatedCallback();
	void setRenderWindowFactory(RenderWindowFactory *factory);

	virtual void Execute(vtkObject *view, unsigned long eventId, void*cbo);

private:
	RenderWindowFactory *mRenderWindowFactory;

};

}//cx
#endif // CXSHAREDCONTEXTCREATEDCALLBACK_H
