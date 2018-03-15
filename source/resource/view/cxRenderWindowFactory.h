/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXRENDERWINDOWFACTORY_H
#define CXRENDERWINDOWFACTORY_H

#include "cxResourceVisualizationExport.h"

#include <map>
#include <QString>
#include <boost/smart_ptr.hpp>
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxSharedContextCreatedCallback.h"

class QVTKWidget;

namespace cx
{
typedef boost::shared_ptr<class RenderWindowFactory> RenderWindowFactoryPtr;

/** \brief Use to create all vtkRenderWindows, and store a single shared render window.
 *
 * Only used directly by ViewService
 *
 *  \ingroup cx_resource_view
 *  \date 2016-11-21
 *  \author Ole Vegard Solberg, SINTEF
 */
class cxResourceVisualization_EXPORT RenderWindowFactory : public QObject
{
	Q_OBJECT
public:
	RenderWindowFactory();
	vtkRenderWindowPtr getRenderWindow(QString uid, bool offScreenRendering = false);
	vtkRenderWindowPtr getSharedRenderWindow() const;

	SharedOpenGLContextPtr getSharedOpenGLContext() const;
	bool renderWindowExists(QString uid);

private:
	//Important: keep private, only the SharedContextCreatedCallback::Execute should be able to set the shared render window.
	void setSharedRenderWindow(vtkRenderWindowPtr sharedRenderWindow);
	friend void SharedContextCreatedCallback::Execute(vtkObject *view, unsigned long eventId, void*cbo);

	vtkRenderWindowPtr createRenderWindow(QString uid, bool offScreenRendering);
	void preventSharedContextRenderWindowFromBeingShownOnScreen(vtkRenderWindowPtr renderWindow);

	vtkRenderWindowPtr mSharedRenderWindow;
	std::map<QString, vtkRenderWindowPtr> mRenderWindows;
	SharedContextCreatedCallbackPtr mSharedContextCreatedCallback;
	SharedOpenGLContextPtr mSharedOpenGLContext;
	QVTKWidget* mQvtkWidgetForHidingSharedContextRenderWindow;

};
}//cx

#endif // CXRENDERWINDOWFACTORY_H
