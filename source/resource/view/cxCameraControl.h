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

/*
 * cxCameraControl.h
 *
 *  \date Oct 15, 2010
 *      \author christiana
 */

#ifndef CXCAMERACONTROL_H_
#define CXCAMERACONTROL_H_

#include "cxResourceVisualizationExport.h"

#include <vector>
#include "cxVector3D.h"
//#include "cxDoubleWidgets.h"
#include "cxForwardDeclarations.h"
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"

class QActionGroup;
class QAction;
class QDomNode;

namespace cx
{
/**
* \file
* \addtogroup cx_resource_view
* @{
*/

typedef boost::shared_ptr<class CameraData> CameraDataPtr;

/** Class encapsulating the view transform of a camera. Use with vtkCamera
 */
class cxResourceVisualization_EXPORT CameraData
{
public:
	CameraData();
	static CameraDataPtr create()
	{
		return CameraDataPtr(new CameraData());
	}
	void setCamera(vtkCameraPtr camera);
	vtkCameraPtr getCamera() const;

	void addXml(QDomNode dataNode) const; ///< store internal state info in dataNode
	void parseXml(QDomNode dataNode);///< load internal state info from dataNode

private:
	mutable vtkCameraPtr mCamera;
	void addTextElement(QDomNode parentNode, QString name, QString value) const;
};

/**Utility class for 3D camera control.
 *
 */
class cxResourceVisualization_EXPORT CameraControl: public QObject
{
Q_OBJECT

public:
	CameraControl(QObject* parent = NULL);
	virtual ~CameraControl();

	void refreshView(ViewPtr view);
	void setView(ViewPtr view);
	QActionGroup* createStandard3DViewActions();
	void translateByFocusTo(Vector3D p_r);
	void setSuperiorView();

signals:

protected slots:
	void setStandard3DViewActionSlot();

private:
	vtkRendererPtr getRenderer() const;
	vtkCameraPtr getCamera() const;
	void defineRotateLayout();
	void definePanLayout();
	ViewPtr mView;
	QAction* mSuperiorViewAction;

	QAction* addStandard3DViewAction(QString caption, QString help, Vector3D viewDirection, QActionGroup* group);
};

/**
* @}
*/
}//end namespace cx

#endif /* CXCAMERACONTROL_H_ */
