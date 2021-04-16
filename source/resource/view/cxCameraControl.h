/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

	static Vector3D AnteriorDirection();
	static Vector3D PosteriorDirection();
	static Vector3D SuperiorDirection();
	static Vector3D InferiorDirection();
	static Vector3D LeftDirection();
	static Vector3D RightDirection();
	static Vector3D OrthogonalDirection();

	void refreshView(ViewPtr view);
	void setView(ViewPtr view);
	ViewPtr getView() const;
	QActionGroup* createStandard3DViewActions();
	void translateByFocusTo(Vector3D p_r);
	void setSuperiorView() const;
	void setAnteriorView() const;

	void setStandard3DView(Vector3D viewDirection);

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
	QAction* mAnteriorViewAction;

	QAction* addStandard3DViewAction(QString caption, QString help, Vector3D viewDirection, QActionGroup* group);
};

/**
* @}
*/
}//end namespace cx

#endif /* CXCAMERACONTROL_H_ */
