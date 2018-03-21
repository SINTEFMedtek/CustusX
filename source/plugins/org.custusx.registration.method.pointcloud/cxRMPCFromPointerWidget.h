/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXRMPCFROMPOINTERWIDGET_H
#define CXRMPCFROMPOINTERWIDGET_H

#include <QPushButton>
#include <QDomElement>
#include "cxRegistrationBaseWidget.h"
#include "cxForwardDeclarations.h"
#include "cxXmlOptionItem.h"
#include "cxICPRegistrationBaseWidget.h"
#include "cxRegistrationProperties.h"

namespace cx
{
class WidgetObscuredListener;
class RecordTrackingWidget;
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
 * \brief Register a point cloud to a surface
 *
 * \date 2015-09-06
 * \author Christian Askeland
 */
class RMPCFromPointerWidget: public ICPRegistrationBaseWidget
{
	Q_OBJECT

public:
	RMPCFromPointerWidget(RegServicesPtr services, QWidget *parent);
	virtual ~RMPCFromPointerWidget() {}
	virtual QString defaultWhatsThis() const;

protected:
	virtual void initializeRegistrator();
	virtual void inputChanged();
	virtual void applyRegistration(Transform3D delta);
	virtual void onShown();
	virtual void setup();

	virtual double getDefaultAutoLTS() const { return false; }

private:
	void queuedAutoRegistration();
	void connectAutoRegistration();
	MeshPtr getTrackerDataAsMesh();
	QVBoxLayout* createVBoxInGroupBox(QVBoxLayout* parent, QString header);

	StringPropertyRegistrationFixedImagePtr mFixedImage;
	RecordTrackingWidget* mRecordTrackingWidget;

	SpaceListenerPtr mSpaceListenerMoving;
	SpaceListenerPtr mSpaceListenerFixed;
};

} //namespace cx

#endif // CXRMPCFROMPOINTERWIDGET_H
