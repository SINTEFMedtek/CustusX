/*
 * cxCameraControl.h
 *
 *  Created on: Oct 15, 2010
 *      Author: christiana
 */

#ifndef CXCAMERACONTROL_H_
#define CXCAMERACONTROL_H_

#include <vector>
#include "sscVector3D.h"
//#include "sscDoubleWidgets.h"
#include "sscForwardDeclarations.h"
#include "vtkForwardDeclarations.h"

class QActionGroup;
class QAction;
class QDomNode;

namespace cx
{

typedef boost::shared_ptr<class CameraData> CameraDataPtr;

/** Class encapsulating the view transform of a camera. Use with vtkCamera
 */
class CameraData
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
class CameraControl: public QObject
{
Q_OBJECT

public:
	CameraControl(QObject* parent = NULL);
	virtual ~CameraControl();

	QActionGroup* createStandard3DViewActions();
	void translateByFocusTo(ssc::Vector3D p_r);

signals:

protected slots:
	void setStandard3DViewActionSlot();

private:
	vtkRendererPtr getRenderer() const;
	vtkCameraPtr getCamera() const;
	void defineRotateLayout();
	void definePanLayout();

	QAction* addStandard3DViewAction(QString caption, QString help, ssc::Vector3D viewDirection, QActionGroup* group);
};

}//end namespace cx

#endif /* CXCAMERACONTROL_H_ */
