/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXRMPCWIDGET_H
#define CXRMPCWIDGET_H

#include <QPushButton>
#include <QDomElement>
#include "cxRegistrationBaseWidget.h"
#include "cxForwardDeclarations.h"
#include "cxXmlOptionItem.h"
#include "cxICPRegistrationBaseWidget.h"

namespace cx
{
class WidgetObscuredListener;
class ICPWidget;
typedef boost::shared_ptr<class Acquisition> AcquisitionPtr;
typedef boost::shared_ptr<class StringPropertySelectMesh> StringPropertySelectMeshPtr;
typedef boost::shared_ptr<class ToolRep3D> ToolRep3DPtr;
typedef boost::shared_ptr<class RecordSessionWidget> RecordSessionWidgetPtr;
typedef boost::shared_ptr<class AcquisitionData> AcquisitionDataPtr;
//typedef boost::shared_ptr<class BronchoscopyRegistration> BronchoscopyRegistrationPtr;
typedef std::map<QString, ToolPtr> ToolMap;
typedef boost::shared_ptr<class StringPropertySelectTool> StringPropertySelectToolPtr;
typedef boost::shared_ptr<class SeansVesselReg> SeansVesselRegPtr;
typedef boost::shared_ptr<class MeshInView> MeshInViewPtr;
typedef boost::shared_ptr<class SpaceListener> SpaceListenerPtr;

/**
 *
 * Register a point cloud in patient space to a point cloud in reference space.
 *
 * The moving data are assumed to be in patient space, and are used to correct
 * the patient registration prMt. The moving data iself are also affected, in order
 * to keep the relation of moving relative to patient.
 *
 * \date 2015-09-16
 * \author Christian Askeland
 */
class RMPCWidget: public ICPRegistrationBaseWidget
{
	Q_OBJECT

public:
	RMPCWidget(RegServicesPtr services, QWidget *parent);
	virtual ~RMPCWidget() {}

protected:
	virtual void initializeRegistrator();
	virtual void inputChanged();
	virtual void applyRegistration(Transform3D delta);
	virtual void onShown();
	virtual void setup();

	virtual double getDefaultAutoLTS() const { return false; }

private:
	StringPropertyBasePtr mFixedImage;
	StringPropertyBasePtr mMovingImage;

	SpaceListenerPtr mSpaceListenerMoving;
	SpaceListenerPtr mSpaceListenerFixed;
};

} //namespace cx

#endif // CXRMPCWIDGET_H
